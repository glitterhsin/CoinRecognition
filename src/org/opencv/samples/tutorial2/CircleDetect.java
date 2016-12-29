package org.opencv.samples.tutorial2;


import java.util.List;

import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.core.Mat;

import android.os.AsyncTask;



public class CircleDetect extends AsyncTask<CvCameraViewFrame, Integer, Mat>{
	public native CircleInfo[] HoughCircle(long matAddrRgba);
	//public native void DrawCircle(long matAddrRgba, CircleInfo[] list);
	
	Mat src;
	Tutorial2Activity ta;
	
	public CircleDetect(Tutorial2Activity ta){
		this.ta = ta;
	}
	
	@Override
	protected void onPreExecute (){
		super.onPreExecute();
	}
	
	@Override
	protected void onProgressUpdate (Integer... values){
		super.onProgressUpdate(values);
	}
	
	@Override
	protected Mat doInBackground(CvCameraViewFrame... params) {
		
		/* src image */
		src = params[0].rgba();
		
		/* call jni HoughCircle */
		ta.circle_list_tmp = HoughCircle(src.getNativeObjAddr());
		
		return src;
	}
	
	@Override
	protected void onCancelled (){
		super.onCancelled();
		
	}
	
	@Override
	protected void onPostExecute (Mat result){
		super.onPostExecute(result);
		
		/* call jni draw */
		//DrawCircle(result.getNativeObjAddr(), circle_list);
		
	}
}
