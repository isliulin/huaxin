package gbpe.policeass.utils;

public class ClickUtil {
	private static long lastClickTime = -1;

	public static boolean isFastClick(long interval) {
		long time = System.currentTimeMillis();
		if (time - lastClickTime < interval) {
			lastClickTime = time;
			return true;
		}
		lastClickTime = time;
		return false;
	}

}
