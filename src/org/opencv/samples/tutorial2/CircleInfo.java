package org.opencv.samples.tutorial2;



public class CircleInfo {
	public float x, y, z;
	public int coin_idx, x_left_top, y_left_top, x_right_btm, y_right_btm;
	public CircleInfo(){}
	public void set(float x, float y, float z, int coin_idx, int x_left_top, int y_left_top, int x_right_btm, int y_right_btm){
		this.x = x;
		this.y = y;
		this.z = z;
		this.coin_idx = coin_idx;
		this.x_left_top = x_left_top;
		this.y_left_top = y_left_top;
		this.x_right_btm = x_right_btm;
		this.y_right_btm = y_right_btm;
	}
}
