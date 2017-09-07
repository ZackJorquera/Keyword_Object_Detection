package KeywordObjectDetectionPackage;

import java.util.*;
import org.opencv.core.*;
import org.opencv.imgcodecs.Imgcodecs;

public class Trainer 
{
	OpenCVTools cvTools = new OpenCVTools();
	
	public Trainer() {}
	
	public int Train(String arg)
	{
		Scanner scanner = new Scanner(System.in); 
		
		String objectName;
		List<String> imageFiles;
		List<Mat> grayScaleimages;
		 
		System.out.println("Info: the center of the image will be used as the center of the object. for the best results please crop the object to the edges of the image so that only the object is classifyed as the object.");
		
		imageFiles = GetInputImages(scanner);

		System.out.print("please ender the name of the object in the images: ");
		objectName = scanner.nextLine();

		grayScaleimages = cvTools.LoadImages(imageFiles, Imgcodecs.IMREAD_GRAYSCALE);
		
		return 0;
	}
	
	private List<String> GetInputImages(Scanner s)
	{
		List<String> imageFiles = new ArrayList<String>();
		System.out.print("\nHow many images are going to be used to train with: ");

		int num;
		num = s.nextInt();

		for (int i = 0; i < num; i++)
		{
			String thisImage = "";
			System.out.print("Image File Location " + (i + 1) + ": ");
			while (thisImage.equals(""))
			{
				thisImage = "";
				thisImage = s.nextLine();
				if(!thisImage.equals(""))
					imageFiles.add(thisImage);
			}
		}
		return imageFiles;
	}
}
