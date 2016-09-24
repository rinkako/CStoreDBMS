using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace TREEDRAW
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private Dictionary<string, tNode> nodeDict = new Dictionary<string, tNode>();

        List<tNode> rootNodeList = new List<tNode>();

        private void button1_Click(object sender, EventArgs e)
        {
            FileStream fs = new FileStream("mylist.csv", FileMode.Open);
            StreamReader sr = new StreamReader(fs);
            // jump first line
            sr.ReadLine();
            while (sr.EndOfStream == false)
            {
                string aline = sr.ReadLine();
                string[] lineitem = aline.Split(',');
                tNode newNode;
                if (nodeDict.ContainsKey(lineitem[1]) == false)
                {
                    newNode = new tNode(lineitem[1]);
                    nodeDict[lineitem[1]] = newNode;
                }
                else
                {
                    newNode = nodeDict[lineitem[1]];
                }
                // if exist promoter
                if (lineitem[4] != "#N/A")
                {
                    if (nodeDict.ContainsKey(lineitem[4]) == false)
                    {
                        nodeDict[lineitem[4]] = new tNode(lineitem[4]);
                    }
                    nodeDict[lineitem[4]].AddChild(newNode);
                    newNode.parent = nodeDict[lineitem[4]];
                }
                // director
                else
                {
                    if (nodeDict.ContainsKey(lineitem[3]) == false)
                    {
                        nodeDict[lineitem[3]] = new tNode(lineitem[3]);
                    }
                    nodeDict[lineitem[3]].AddChild(newNode);
                    newNode.parent = nodeDict[lineitem[3]];
                }
            }
            sr.Close();
            fs.Close();
            // search Nulls
            foreach (var t in this.nodeDict)
            {
                if (t.Value.parent == null)
                {
                    rootNodeList.Add(t.Value);
                }
            }
        }

        public void DrawTree()
        {
            string outStr = "";
            foreach (var rt in this.rootNodeList)
            {
                TreeNode myviewroot = this.treeView1.Nodes.Add(rt.name);
                outStr += rt.name + Environment.NewLine;
                outStr += RecursiveDrawTree(rt, myviewroot);
                outStr += Environment.NewLine;
            }
            FileStream fs = new FileStream("out.txt", FileMode.OpenOrCreate);
            StreamWriter sw = new StreamWriter(fs);
            sw.Write(outStr);
            sw.Close();
            fs.Close();
        }

        public string RecursiveDrawTree(tNode root, TreeNode rootViewNode)
        {
            if (root == null || rootViewNode == null) { return String.Empty; }
            string offsetSpace = String.Empty;
            string outStr = String.Empty;
            for (int i = 0; i <= rootViewNode.Level; i++)
            {
                offsetSpace += "---";
            }
            foreach (var t in root.children)
            {
                TreeNode vr = rootViewNode.Nodes.Add(t.name);
                outStr += offsetSpace + t.name + Environment.NewLine;
                outStr += this.RecursiveDrawTree(t, vr);
            }
            return outStr;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.DrawTree();
            this.treeView1.ExpandAll();
            this.textBox1.Text = "";
            int sum = 0;
            foreach (var t in rootNodeList)
            {
                sum += t.CountChildren();
                this.textBox1.Text += t.CountChildren().ToString() + Environment.NewLine;
            }
            this.textBox1.Text += "-----" + Environment.NewLine;
            this.textBox1.Text += "Total:" + sum.ToString();
        }
    }

    public class tNode
    {
        public tNode(string tn)
        {
            this.name = tn;
        }

        public void AddChild(tNode child)
        {
            this.children.Add(child);
        }

        public string name;

        public tNode parent = null;

        public List<tNode> children = new List<tNode>();

        public override string ToString()
        {
            return String.Format("{0} ({1}) --> {2}", this.name, this.children.Count, this.parent == null ? "NULL" : this.parent.name);
        }

        public int CountChildren()
        {
            if (this.children.Count == 0)
            {
                return 1;
            }
            int sum = 0;
            foreach (var t in this.children)
            {
                sum += t.CountChildren();
            }
            return 1 + sum;
        }
    }
}
