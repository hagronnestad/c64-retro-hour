<Query Kind="Statements">
  <NuGetReference>System.ServiceModel.Syndication</NuGetReference>
  <Namespace>System.Net.Http</Namespace>
  <Namespace>System.ServiceModel.Syndication</Namespace>
</Query>

XmlReader reader = XmlReader.Create("https://audioboom.com/channels/4970769.rss");
SyndicationFeed feed = SyndicationFeed.Load(reader);
reader.Close();

var titles = new List<string>();

var cnt = 0;

foreach (SyndicationItem item in feed.Items.Reverse())
{
	var title = Regex.Replace(item.Title.Text, "The Retro Hour - Episode.*\\(", "");
	title = Regex.Replace(title, " - The Retro Hour EP.*", "");
	title = Regex.Replace(title, " – The Retro Hour EP.*", "");
	title = Regex.Replace(title, "[0-9]*:", "");
	title = title.Replace("’", "'");
	title = title.TrimEnd(')');
	title = title.Trim();
	
	titles.Add(title);
	cnt++;
}

var sb = new StringBuilder();
sb.Append("// Automatically generated by CreateEpisodesHeaderFile.linq\n");
sb.Append($"static unsigned int episodes_count = {cnt};\n");
sb.Append($"static char* episodes_update_date = \"{DateTime.UtcNow:yyyy-MM-dd}\";\n");
sb.Append("static char* episodes[] = {\n");
foreach (var e in titles)
{
	sb.Append($"    \"{e.Replace("\"", "\\\"")}\",\n");
}
sb.Append("};\n");

sb.ToString().Dump();