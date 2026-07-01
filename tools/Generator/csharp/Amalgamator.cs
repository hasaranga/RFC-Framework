using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Generator
{
    class HeaderFile : IComparable<HeaderFile>
    {
        public String filePath; // complete path
        public String fileBuff; // file content without #include "xxx.h"
        public List<String> incList; // include list with complete path

        public HeaderFile()
        {
            filePath = "";
            fileBuff = "";
            incList = new List<String>();
        }

        public int CompareTo(HeaderFile hf) // require for sorting...
        {
            if (hf.incList.Count > incList.Count)
            {
                return -1;
            }
            else if (hf.incList.Count == incList.Count)
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
    }

    public class Amalgamator
    {
        static int lineCount;
        static HeaderFile ReadHeaderFile(string path)
        {
            Console.Out.WriteLine("Processing: " + path);

            HeaderFile headerFile = new HeaderFile();
            headerFile.filePath = path;

            StreamReader sr = new StreamReader(path);

            Regex regex = new Regex("#include *\"(_*/*.*[0-9]*[A-Z]*[a-z]*)+\""); // file name can contain _/.number or char
            Regex regex2 = new Regex("\"(_*/*.*[0-9]*[A-Z]*[a-z]*)+\"");

            String curLine;
            while ((curLine = sr.ReadLine()) != null)
            {
                Match m = regex.Match(curLine);
                if (m.Success)
                {
                    String includeSection = curLine.Substring(m.Index, m.Length);
                    m = regex2.Match(includeSection);
                    String relativeHeaderPath = includeSection.Substring(m.Index, m.Length).Substring(1, m.Length - 2);

                    String curDir = Path.GetDirectoryName(path);
                    String incFilePath = Path.GetFullPath(curDir + "/" + relativeHeaderPath);
                    if (File.Exists(incFilePath))
                    {
                        headerFile.incList.Add(incFilePath);
                    }
                    else
                    {
                        Console.Out.WriteLine("Error: missing " + incFilePath);
                        return null;
                    }
                }
                else
                {
                    lineCount++;
                    headerFile.fileBuff += "\r\n" + curLine;
                }
            }

            sr.Close();
            return headerFile;
        }

        static List<HeaderFile> SerachForHeaderFiles(string sourceDir)
        {
            string[] fileEntries = Directory.GetFiles(Path.GetFullPath(sourceDir), "*.h", SearchOption.AllDirectories);

            List<HeaderFile> headerFileList = new List<HeaderFile>();

            foreach (string fileName in fileEntries)
            {
                HeaderFile hf = ReadHeaderFile(fileName);
                if (hf != null)
                {
                    headerFileList.Add(hf);
                }
                else
                {
                    return null;
                }
            }

            return headerFileList;
        }

        static bool IsRequiredHeadersWritten(HeaderFile headerFile, List<HeaderFile> WrittenFiles)
        {
            bool allWritten = true;
            for (int i = 0; i < headerFile.incList.Count; i++)
            {
                bool curFileFound = false;
                for (int j = 0; j < WrittenFiles.Count; j++)
                {
                    if (WrittenFiles[j].filePath.CompareTo(headerFile.incList[i]) == 0)
                    {
                        curFileFound = true;
                        break;
                    }
                }
                if (!curFileFound)
                {
                    allWritten = false;
                    break;
                }
            }
            return allWritten;
        }

        static bool AmalgamateHeaderFiles(String frameworkPath, String frameworkName)
        {
            List<HeaderFile> headerFileList = SerachForHeaderFiles(frameworkPath);

            if (headerFileList == null)
                return false;

            headerFileList.Sort();

            String outHeaderFile = "\r\n// ========== RFC Generator v1.0 - "+ DateTime.Now.ToString("yyyy-MM-dd HH:mm tt") + " ==========\r\n";

            String tmp = "_" + frameworkName.Replace(' ', '_').ToUpper();
            outHeaderFile += "\r\n#ifndef " + tmp + "_H_\r\n#define " + tmp + "_H_ \r\n\r\n#define AMALGAMATED_VERSION\r\n";

            List<HeaderFile> WrittenFiles = new List<HeaderFile>();

            Console.Out.WriteLine("\n");

            // get 0 includes
            foreach (HeaderFile headerFile in headerFileList)
            {
                if (headerFile.incList.Count == 0)
                {
                    Console.Out.WriteLine("Adding: " + headerFile.filePath);
                    outHeaderFile += "\r\n\r\n// =========== " + Path.GetFileName(headerFile.filePath) + " ===========" + headerFile.fileBuff;
                    WrittenFiles.Add(headerFile);
                }
                else
                {
                    break;
                }
            }

            if (WrittenFiles.Count != 0) // no circular dependancies
            {
                // remove 0 includes
                foreach (HeaderFile headerFile in WrittenFiles)
                {
                    headerFileList.Remove(headerFile);
                }

                while (headerFileList.Count != 0)
                {
                    foreach (HeaderFile headerFile in headerFileList)
                    {
                        if (headerFile.incList.Count <= WrittenFiles.Count)
                        {
                            if (IsRequiredHeadersWritten(headerFile, WrittenFiles))
                            {
                                Console.Out.WriteLine("Adding: " + headerFile.filePath);
                                outHeaderFile += "\r\n\r\n// =========== " + Path.GetFileName(headerFile.filePath) + " ===========" + headerFile.fileBuff;
                                WrittenFiles.Add(headerFile);
                            }
                        }
                        else
                        {
                            break;
                        }
                    }

                    // remove written files from the list
                    foreach (HeaderFile headerFile in WrittenFiles)
                    {
                        headerFileList.Remove(headerFile);
                    }
                }

                outHeaderFile += "\r\n\r\n#endif\r\n\r\n";

                Console.Out.WriteLine("\nWriting: " + frameworkName + ".h lines: " + lineCount + "\n");
                StreamWriter sw = new StreamWriter(frameworkName + ".h");
                sw.Write(outHeaderFile);
                sw.Close();

                return true;
            }
            else
            {
                Console.Out.WriteLine("\nError: Circular dependancy detected!");
                return false;
            }
        }

        public static bool MakeAmalgamatedRFC(string frameworkPath, string outputName)
        {
            if(AmalgamateHeaderFiles(frameworkPath, outputName))
            {
                AmalgamateSourceFiles(frameworkPath, outputName);
                return true;
            }
            return false;
        }

        static void AmalgamateSourceFiles(String frameworkPath, String frameworkName)
        {
            string[] fileEntries = Directory.GetFiles(frameworkPath, "*.cpp", SearchOption.AllDirectories);

            String outSourceFile = "\r\n// ========== RFC Generator v1.0 ==========\r\n";

            outSourceFile += "\r\n\r\n#include \"" + frameworkName + ".h\"\r\n";

            int lineCount = 0;

            foreach (string fileName in fileEntries)
            {
                Console.Out.WriteLine("Adding: " + fileName);

                StreamReader sr = new StreamReader(fileName);

                outSourceFile += "\r\n\r\n// =========== " + Path.GetFileName(fileName) + " ===========";

                Regex regex = new Regex("#include *\"(_*/*.*[0-9]*[A-Z]*[a-z]*)+\""); // file name can contain _/.number or char

                String curLine;
                while ((curLine = sr.ReadLine()) != null)
                {
                    lineCount++;
                    Match m = regex.Match(curLine);
                    if (!m.Success) // bypass #includes...
                    {
                        outSourceFile += "\r\n" + curLine;
                    }
                }

                sr.Close();
            }

            Console.Out.WriteLine("\nWriting: " + frameworkName + ".cpp lines: " + lineCount + "\n");
            StreamWriter sw = new StreamWriter(frameworkName + ".cpp");
            sw.Write(outSourceFile);
            sw.Close();

        }
    }
}
