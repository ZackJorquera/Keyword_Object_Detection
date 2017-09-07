package KeywordObjectDetectionPackage;

import KeywordObjectDetectionPackage.Trainer;

public class Source 
{
	public static void main(String[] args)//Does java count the first arg as the file when called in the cmd 
	{
		if (args.length < 2)
		{
			System.out.println("Use \"train\" to select training mode and \"find\" to select detection Mode");
			System.exit(0);
			return;
		}

		if (args[1].equals("train"))
		{
			Trainer tr = new Trainer();
			System.out.println("Training Mode:");
			if (args.length > 2)
				tr.Train(args[2]);
			else
				tr.Train("");
		}
		else if (args[1].equals("find"))
		{
			Detector de = new Detector();
			System.out.println("Detection Mode:");
			de.Find("");
		}
		else
		{
			System.out.println("Invalade selection \"" + args[1] + "\":\nSelection input is case sensitive and all lower case");
			System.exit(0);
			return;
		}	
		System.exit(0);
		return;
	}
}