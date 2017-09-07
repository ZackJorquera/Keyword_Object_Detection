package KeywordObjectDetectionPackage;

import java.util.*;
import org.opencv.core.*;
import org.opencv.imgcodecs.Imgcodecs;

public class OpenCVTools 
{
	public OpenCVTools() 
	{
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
	}
	
	public List<Mat> LoadImages(List<String> paths, int imreadCodec)
	{
		List<Mat> imgs = new ArrayList<Mat>();
		for (int i = 0; i < paths.size(); i++)
		{
			if (paths.get(i).equals(""))
				continue;
			Mat temp = Imgcodecs.imread(paths.get(i), imreadCodec);
			if(temp.dims() != 0)
				imgs.add(temp);
		}
		return imgs;
	}
}
