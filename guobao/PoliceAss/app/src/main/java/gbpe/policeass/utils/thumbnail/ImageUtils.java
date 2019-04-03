package gbpe.policeass.utils.thumbnail;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.media.MediaMetadataRetriever;
import android.media.MediaScannerConnection;
import android.media.ThumbnailUtils;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import gbpe.policeass.utils.MetadataUtil;

/**
 * 图片简单处理工具类
 */
public class ImageUtils {
	static String TAG = "ImageUtils";
	
	/**
	 * 屏幕宽
	 * 
	 * @param context
	 * @return
	 */
	public static int getWidth(Context context) {
		DisplayMetrics dm = context.getResources().getDisplayMetrics();
		return dm.widthPixels;
	}

	/**
	 * 屏幕高
	 * 
	 * @param context
	 * @return
	 */
	public static int getHeight(Context context) {
		DisplayMetrics dm = context.getResources().getDisplayMetrics();
		return dm.heightPixels;
	}
	
	/**
	 * 根据文件Uri获取路径
	 * 
	 * @param context
	 * @param uri
	 * @return
	 */
	public static String getFilePathByFileUri(Context context, Uri uri) {
		String filePath = null;
		Cursor cursor = context.getContentResolver().query(uri, null, null,
				null, null);
		if (cursor.moveToFirst()) {
			filePath = cursor.getString(cursor
					.getColumnIndex(MediaStore.Images.Media.DATA));
		}
		cursor.close();
		return filePath;
	}

	/**
	 * 根据图片原始路径获取图片缩略图
	 * 
	 * @param imagePath 图片原始路径
	 * @param width		缩略图宽度
	 * @param height	缩略图高度
	 * @return
	 */
	public static Bitmap getImageThumbnail(String imagePath, int width,
										   int height) {
		Bitmap bitmap = null;
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inJustDecodeBounds = true;//不加载直接获取Bitmap宽高
		// 获取这个图片的宽和高，注意此处的bitmap为null
		bitmap = BitmapFactory.decodeFile(imagePath, options);
		if(bitmap == null){
		// 计算缩放比
		int h = options.outHeight;
		int w = options.outWidth;
		int beWidth = w / width;
		int beHeight = h / height;
		int rate = 1;
		if (beWidth < beHeight) {
			rate = beWidth;
		} else {
			rate = beHeight;
		}
		if (rate <= 0) {//图片实际大小小于缩略图,不缩放
			rate = 1;
		}
		options.inSampleSize = rate;
		options.inJustDecodeBounds = false; 
		// 重新读入图片，读取缩放后的bitmap，注意这次要把options.inJustDecodeBounds 设为 false
		bitmap = BitmapFactory.decodeFile(imagePath, options);
		// 利用ThumbnailUtils来创建缩略图，这里要指定要缩放哪个Bitmap对象
		bitmap = ThumbnailUtils.extractThumbnail(bitmap, width, height,
				ThumbnailUtils.OPTIONS_RECYCLE_INPUT);
		}
		return bitmap;
	}
	public  static  Bitmap getVideoThumbnail(String filePath) {
		Bitmap bitmap = null;
		MediaMetadataRetriever retriever = new MediaMetadataRetriever();
		try {
			retriever.setDataSource(filePath);
			bitmap = retriever.getFrameAtTime();
		}
		catch(IllegalArgumentException e) {
			e.printStackTrace();
		}
		catch (RuntimeException e) {
			e.printStackTrace();
		}
		finally {
			try {
				retriever.release();
			}
			catch (RuntimeException e) {
				e.printStackTrace();
			}
		}
		return bitmap;
	}


	static public Bitmap getVideoThumbnail(ContentResolver cr, String path) {
		Bitmap bitmap = null;
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inDither = false;
		options.inPreferredConfig = Bitmap.Config.RGB_565;
		//select condition.
		String whereClause = MediaStore.Video.Media.DATA + " = '"
				+ path + "'";
		Log.v(TAG, "where = " + whereClause);
		//colection of results.
		Cursor cursor = cr.query(MediaStore.Video.Media.EXTERNAL_CONTENT_URI,
				new String[] { MediaStore.Video.Media._ID }, whereClause,
				null, null);
		Log.v(TAG, "cursor = " + cursor);
		if (cursor == null || cursor.getCount() == 0) {
			return ThumbnailUtils.createVideoThumbnail(path, MediaStore.Video.Thumbnails.MINI_KIND);
		}
		cursor.moveToFirst();
		//image id in image table.
		String videoId = cursor.getString(cursor
				.getColumnIndex(MediaStore.Video.Media._ID));
		Log.v(TAG, "videoId = " + videoId);
		if (videoId == null) {
			return ThumbnailUtils.createVideoThumbnail(path, MediaStore.Video.Thumbnails.MINI_KIND);
		}
		cursor.close();
		long videoIdLong = Long.parseLong(videoId);
		//via imageid get the bimap type thumbnail in thumbnail table.
		bitmap = MediaStore.Video.Thumbnails.getThumbnail(cr, videoIdLong,
				MediaStore.Images.Thumbnails.MICRO_KIND, options);
		Log.v(TAG, "bitmap = " + bitmap);
		return bitmap;
	}
	/**
	 * @Description 定宽高解码缩略图
	 * @param path 文件路径
	 * @param width 显示的宽度
	 * @param height 显示的高度
	 * @return Bitmap 缩略图
	 */
	public static Bitmap decodeThumbBitmapForFile(final String path, final int width, final int height) {
		Bitmap bitmap = ThumbnailUtils.createVideoThumbnail(path, MediaStore.Images.Thumbnails.FULL_SCREEN_KIND); //还可以选择MINI_KIND和MICRO_KIND
		if (bitmap != null) {
			bitmap = ThumbnailUtils.extractThumbnail(bitmap, width, height, ThumbnailUtils.OPTIONS_RECYCLE_INPUT); //根据源图片指定宽高生成新的图片
		}
		return bitmap;
	}
	public static List<Bitmap>   get(Context context){
		List<Bitmap> bitmaps = new ArrayList<>();


		//	Uri originalUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
		Uri originalUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
		//若为视频则为MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
		ContentResolver cr = context.getContentResolver();
		Cursor cursor = cr.query(originalUri, null, null, null, null);
		if (cursor == null) {
			return null;
		}
		for (cursor.moveToFirst();!cursor.isAfterLast(); cursor.moveToNext()) {
			long thumbNailsId = cursor.getLong(cursor   .getColumnIndex("_ID"));
//			Bitmap bitmap = MediaStore.Images.Thumbnails.getThumbnail(cr,
//					thumbNailsId, MediaStore.Video.Thumbnails.MICRO_KIND, null);
			//若为视频则为
                  Bitmap bitmap = MediaStore.Video.Thumbnails.getThumbnail(cr,
                          thumbNailsId, MediaStore.Video.Thumbnails.MICRO_KIND, null);
			             bitmaps.add(bitmap);
		}
		return bitmaps;
	}
	/**
	 * 通过文件名 获取视频的缩略图
	 *
	 * @param context
	 * @param cr     cr = getContentResolver();
	 * @param testVideopath  全路径 "/mnt/sdcard/sidamingbu.mp4";
	 * @return
	 */
	public static Bitmap getVideoThumbnail(Context context, ContentResolver cr, String testVideopath) {


		// final String testVideopath = "/mnt/sdcard/sidamingbu.mp4";
		ContentResolver testcr = context.getContentResolver();
		String[] projection = { MediaStore.Video.Media.DATA, MediaStore.Video.Media._ID, };
		String whereClause = MediaStore.Video.Media.DATA + " = '" + testVideopath + "'";
		Cursor cursor = testcr.query(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, projection, whereClause,
				null, null);
		int _id = 0;
		String videoPath = "";
		if (cursor == null || cursor.getCount() == 0) {
			return null;
		}
		if (cursor.moveToFirst()) {

			int _idColumn = cursor.getColumnIndex(MediaStore.Video.Media._ID);
			int _dataColumn = cursor.getColumnIndex(MediaStore.Video.Media.DATA);

			do {
				_id = cursor.getInt(_idColumn);
				videoPath = cursor.getString(_dataColumn);
				System.out.println(_id + " " + videoPath);
			}
			while (cursor.moveToNext());
 		}
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inDither = false;
		options.inPreferredConfig = Bitmap.Config.ARGB_8888;
		Bitmap bitmap = MediaStore.Video.Thumbnails.getThumbnail(cr, _id, MediaStore.Images.Thumbnails.MICRO_KIND,
				options);
		if (!cursor.isClosed()){
			cursor.close();
		}
		return bitmap;
	}

	public static void allScan(final Context context) {//扫描媒体库

		MediaScannerConnection.scanFile(context, new String[] {

						Environment.getExternalStorageDirectory().getAbsolutePath()},

				null, new MediaScannerConnection.OnScanCompletedListener() {

					public void onScanCompleted(String path, Uri uri)
					{
						MetadataUtil.changeImgMetaAdded(context,path);
					}

				});
	}

	public static void updateGallery(final Context context, String filename)//filename是我们的文件全名，包括后缀哦
	{
		MediaScannerConnection.scanFile(context,
				new String[] { filename }, null,
				new MediaScannerConnection.OnScanCompletedListener() {
					public void onScanCompleted(String path, Uri uri) {
						Log.i("ExternalStorage", "Scanned " + path + ":");
						Log.i("ExternalStorage", "-> uri=" + uri);
						System.out.println("-------->ExternalStorage");
						MetadataUtil.changeImgMetaAdded(context,path);
					}
				});
	}
public static void updateFile(final Context context, String fileName, final String time)
{
	MediaScannerConnection.scanFile(context, new String[]{Environment.getExternalStoragePublicDirectory
			(Environment.DIRECTORY_DCIM).getPath() + "/" + fileName}, null, new MediaScannerConnection.MediaScannerConnectionClient() {
		@Override
		public void onMediaScannerConnected() {

		}

		@Override
		public void onScanCompleted(String path, Uri uri) {
			MetadataUtil.changeImgMetaAdded(context,path);
		}
	});
}


	public static void query(Context context,String filePath)
	{
		Cursor cursor = context.getContentResolver().query(MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
			                 new String[]{MediaStore.Audio.Media.DATA}, null, null, null);
		 if (cursor !=null)
		 {
			 cursor.moveToFirst();
			 while (cursor.moveToNext()) {
 				 String data = cursor.getString(cursor.getColumnIndexOrThrow(MediaStore.Video.Media.DATA));
				 if (data.equals(filePath))
				 {
					 MetadataUtil.changeImgMetaAdded(context,filePath);
				 }
			 }
			 cursor.close();
		 }

	}

	/**
	 * 图片按比例压缩
	 * @param filePath
	 * @param Towidth
	 * @param ToHeight
     * @return
     */
	public static Bitmap getImageByFilePath(String filePath, int Towidth, int ToHeight) {
		Bitmap res = null;
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inJustDecodeBounds = true;
		if (!new File(filePath).exists())
			return res;
		BitmapFactory.decodeFile(filePath, options);

		int origionalWidth = options.outHeight;
		int origionalHeight = options.outWidth;
		options.inJustDecodeBounds = false;
		int scale = Math.max(origionalWidth / Towidth, origionalHeight / ToHeight);
		options.inSampleSize = scale;
		options.inPreferredConfig = Bitmap.Config.ARGB_4444;
		try {
			res = BitmapFactory.decodeFile(filePath, options);
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		} catch (OutOfMemoryError e) {
			e.printStackTrace();
			return null;
		}
		return res;
	}

	/**
	 *图片按比例压缩到屏幕大小
	 * @param path
	 * @param activity
	 * @return
	 */
	public static Bitmap scaleLoad(String path, Activity activity) {
		// 得到图片的宽和高
		BitmapFactory.Options opts = new BitmapFactory.Options();
		opts.inJustDecodeBounds = true; // 加载器不加载图片, 而是把图片的out(宽和高)的字段信息取出来
		BitmapFactory.decodeFile(path, opts);
		int imageWidth = opts.outWidth;
		int imageHeight = opts.outHeight;
		Log.e("TAG","图片的宽和高: " + imageWidth + " * " + imageHeight);    // 得到屏幕的宽和高
		Display display = activity.getWindowManager().getDefaultDisplay();
		int screenWidth = display.getWidth();
		int screenHeight = display.getHeight();
		Log.e("TAG","屏幕的宽和高: " + screenWidth + " * " + screenHeight);    // 计算缩放比例
		int widthScale = imageWidth / screenWidth;
		int heightScale = imageHeight / screenHeight;
		int scale = widthScale > heightScale ? widthScale : heightScale;
		System.out.println("缩放比例为: " + scale);       // 使用缩放比例进行缩放加载图片
		opts.inJustDecodeBounds = false; // 加载器就会返回图片了
		opts.inSampleSize = scale;
		Bitmap bm = BitmapFactory.decodeFile(path, opts);
		// 显示在屏幕上
		return bm;
	}



}
