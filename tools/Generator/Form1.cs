
// (c) 2022 CrownSoft

using System.Diagnostics;
using System.Threading.Tasks;
using System.Xml;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Header;

namespace Generator
{
    public partial class Form1 : Form
    {
        List<RFCModule> modules = new List<RFCModule>();
        string currentPath;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            currentPath = System.IO.Path.GetDirectoryName(Application.ExecutablePath);
            string[] dirList = Directory.GetDirectories(currentPath);
            foreach (string dir in dirList)
            {
                string moduleFilePath = dir + "\\module.xml";
                if(File.Exists(moduleFilePath))
                {
                    XmlDataDocument xmldoc = new XmlDataDocument();
                    xmldoc.Load(moduleFilePath);

                    RFCModule module = new RFCModule();
                    module.name = xmldoc.GetElementsByTagName("name").Item(0).InnerText.Trim();
                    module.isFixed = xmldoc.GetElementsByTagName("fixed").Item(0).InnerText.Trim() == "true";
                    string topLevelDeps = xmldoc.GetElementsByTagName("dependencies").Item(0).InnerText.Trim();
                    string[] depArray = topLevelDeps.Length > 0 ? topLevelDeps.Split(',') : null;
                    module.topLevelDependencies = new List<string>();
                    if(depArray != null)
                    {
                        foreach (string depName in depArray)
                            module.topLevelDependencies.Add(depName);
                    }
                    module.platform = xmldoc.GetElementsByTagName("platform").Item(0).InnerText.Trim();
                    module.description = xmldoc.GetElementsByTagName("description").Item(0).InnerText.Trim();
                    module.dir = dir;

                    modules.Add(module);
                    ListViewItem item = listView1.Items.Add(module.name + " Module");
                    item.Tag = module;
                    module.listItem = item;

                    if (module.isFixed)
                    {
                       item.Checked = true;
                       item.ForeColor = SystemColors.GrayText;
                    }
                }
            }

            // get all dependencies
            foreach (RFCModule module in modules)
            {
                module.allDependencies = new List<string>();
                FillAllDependencies(module.name, module.allDependencies);
                module.dependenciesStr = string.Join(",", module.allDependencies); 
            }

            listView1.ItemChecked += OnListItemChecked;
        }

        RFCModule GetModuleByName(string name)
        {
            foreach (RFCModule module in modules)
            {
                if(module.name == name)
                    return module;
            }

            return null;
        }

        void FillAllDependencies(string moduleName, List<string> allDependencies)
        {
            RFCModule module = GetModuleByName(moduleName);
            if(module != null)
            {
                foreach (string depModuleName in module.topLevelDependencies)
                {
                    if(allDependencies.IndexOf(depModuleName) == -1) // new dependency
                    {
                        allDependencies.Add(depModuleName);
                        FillAllDependencies(depModuleName, allDependencies);
                    }
                }
            }
            else
            {
                MessageBox.Show("Missing dependency: " + moduleName, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
            }
        }

        string CheckForMissingDeps()
        {
            foreach (RFCModule module in modules)
            {
                if(module.listItem.Checked)
                {
                    foreach (string depModuleName in module.allDependencies)
                    {
                        RFCModule depModule = GetModuleByName(depModuleName);
                        if(!depModule.listItem.Checked)
                        {
                            return module.name + " module requires " + depModuleName + " module!";
                        }
                    }
                }
            }

            return null;
        }

        private void listView1_ItemSelectionChanged(object sender, ListViewItemSelectionChangedEventArgs e)
        {
            RFCModule module = (RFCModule)e.Item.Tag;
            if (module == null)
                return;

            textBox2.Text = module.description + "\r\n\r\nDependencies: "+ module.dependenciesStr + "\r\nOS Support: " + module.platform;
        }

        void UpdateStatusLabel()
        {
            string stat = CheckForMissingDeps();
            if (stat != null)
            {
                label4.Text = stat;
                label4.ForeColor = Color.Red;
            }
            else
            {
                label4.Text = "no issues";
                label4.ForeColor = Color.Green;
            }
        }

        void OnListItemChecked(object sender, ItemCheckedEventArgs e)
        {
            RFCModule module = (RFCModule)e.Item.Tag;
            if (module == null)
                return;

            // showing msgbox inside of this event sometimes causes checked item to uncheck! (bug)
            // to avoid that, we run the code after this event!
            Task.Delay(0).ContinueWith(_ => {
                Invoke(() =>
                {
                    listView1.ItemChecked -= OnListItemChecked;

                    if (module.isFixed) // prevent uncheck if fixed
                    {
                        e.Item.Checked = true;
                        listView1.ItemChecked += OnListItemChecked;
                        return;
                    }

                    if (e.Item.Checked)
                    {
                        List<RFCModule> newDependencies = new List<RFCModule>();
                        foreach (string depModuleName in module.allDependencies)
                        {
                            RFCModule depModule = GetModuleByName(depModuleName);
                            if (!depModule.listItem.Checked)
                                newDependencies.Add(depModule);
                        }

                        if (newDependencies.Count > 0)
                        {
                            List<string> newDependenciesStrList = new List<string>();
                            foreach (RFCModule depModule in newDependencies)
                            {
                                newDependenciesStrList.Add(depModule.name);
                            }
                            string newDependenciesStr = string.Join(",", newDependenciesStrList);

                            if (MessageBox.Show("Following new dependencies will be added. Do you want to continue?\r\n\r\nDependencies: "
                                + newDependenciesStr, "Confirm", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
                            {
                                foreach (RFCModule depModule in newDependencies)
                                {
                                    depModule.listItem.Checked = true;
                                }
                                e.Item.Checked = true; // just for safe
                            }
                            else
                            {
                                e.Item.Checked = false;
                            }
                        }
                    }
                    else // uncheck
                    {
                        // nothing to do :-)
                    }
                    listView1.ItemChecked += OnListItemChecked;

                    UpdateStatusLabel();
                });
            });         
        }

        private static void CopyDirectory(string root, string dest)
        {
            foreach (var directory in Directory.GetDirectories(root))
            {
                string dirName = Path.GetFileName(directory);
                if (!Directory.Exists(Path.Combine(dest, dirName)))
                {
                    Directory.CreateDirectory(Path.Combine(dest, dirName));
                }
                CopyDirectory(directory, Path.Combine(dest, dirName));
            }

            foreach (var file in Directory.GetFiles(root))
            {
                File.Copy(file, Path.Combine(dest, Path.GetFileName(file)));
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string stat = CheckForMissingDeps();
            if (stat != null)
            {
                MessageBox.Show(stat, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            string outputDir = textBox1.Text;
            if (outputDir.Length == 0)
            {
                MessageBox.Show("Please select the output folder!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            // save config file
            try
            {
                string configFile = outputDir + "\\rfc.conf";
                File.Delete(configFile);

                List<string> moduleNames = new List<string>();

                foreach (RFCModule module in modules)
                {
                    if (module.listItem.Checked)
                    {
                        moduleNames.Add(module.name);
                    }
                }

                File.WriteAllLines(configFile, moduleNames);
            }
            catch (Exception) { }

            button1.Enabled = false;
            button2.Enabled = false;
            listView1.Enabled = false;

            string tmpPath = currentPath + "\\tmp";
            try
            {
                Directory.Delete(tmpPath, true);
            }
            catch (Exception) { }

            Directory.CreateDirectory(tmpPath);

            foreach (RFCModule module in modules)
            {
                if (module.listItem.Checked)
                {
                    string newDir = tmpPath + "\\" + new DirectoryInfo(module.dir).Name;
                    Directory.CreateDirectory(newDir);
                    CopyDirectory(module.dir, newDir);
                }
            }

            bool amalgamatorResult = false;

            try
            {
                amalgamatorResult = Amalgamator.MakeAmalgamatedRFC(tmpPath, "rfc");
            }
            catch (Exception) { }

            /*
            ProcessStartInfo startInfo = new ProcessStartInfo();
            startInfo.FileName = currentPath + "\\amalgamator.exe";
            startInfo.Arguments = "\""+tmpPath+"\" rfc";
            var process = Process.Start(startInfo);
            process.WaitForExit();*/

            try
            {
                Directory.Delete(tmpPath, true);
            }
            catch (Exception) { }

            if (amalgamatorResult)
            {
                File.Move(currentPath + "\\rfc.h", outputDir + "\\rfc.h", true);
                File.Move(currentPath + "\\rfc.cpp", outputDir + "\\rfc.cpp", true);
            }
            else
            {
                try
                {
                    File.Delete(currentPath + "\\rfc.h");
                    File.Delete(currentPath + "\\rfc.cpp");
                }
                catch (Exception) { }
            }

            button1.Enabled = true;
            button2.Enabled = true;
            listView1.Enabled = true;

            if (amalgamatorResult)
                MessageBox.Show("Files were successfully generated!", "Success", MessageBoxButtons.OK, MessageBoxIcon.Information);
            else
                MessageBox.Show("Error occured while processing files!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);

            this.BringToFront();
            this.ActiveControl = button2;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if(textBox1.Text.Length != 0)
                folderBrowserDialog1.InitialDirectory = textBox1.Text;

            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                textBox1.Text = folderBrowserDialog1.SelectedPath;

                string configFile = textBox1.Text + "\\rfc.conf";
                if (File.Exists(configFile))
                {
                    string[] moduleNames = File.ReadAllLines(configFile);
                    listView1.ItemChecked -= OnListItemChecked;

                    // clear old items
                    foreach (RFCModule module in modules)
                    {
                        if (!module.isFixed)
                        {
                            module.listItem.Checked = false;
                        }
                    }

                    foreach (string moduleName in moduleNames)
                    {
                        RFCModule module = GetModuleByName(moduleName);
                        if(module != null)
                        {
                            module.listItem.Checked = true;
                        }
                        else // module not found
                        {
                            MessageBox.Show(moduleName + " Module not found!", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        }
                    }
                    listView1.ItemChecked += OnListItemChecked;
                    UpdateStatusLabel();
                }
            }
        }
    }

    class RFCModule
    {
        public string name;
        public string platform;
        public string description;
        public bool isFixed;
        public List<string> topLevelDependencies;
        public List<string> allDependencies;
        public string dependenciesStr;
        public string dir;
        public ListViewItem listItem;
    }
}