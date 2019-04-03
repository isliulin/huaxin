package gbpe.policeass.zfysim;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Bundle;
import android.os.SystemClock;
import android.os.Vibrator;
import android.provider.Settings;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;



import java.io.IOException;
import java.util.List;
import java.util.Locale;

import gbpe.policeass.R;


public class CompassActivity extends Activity implements SensorEventListener,LocationListener {

	private ImageView img_compass_dial;
	private ImageView img_compass_pointer;
	private RelativeLayout img_compass_bg;
	private TextView cur_degree;
	private TextView cali_text;
	private TextView cur_direction;
	// 经度
	private TextView longitude_section;
	private TextView longitude_degree;
	private static double lastLongitude=0.0;
	// 纬度
	private TextView latitude_section;
	private TextView latitude_degree;
	private static double lastLatitude=0.0;

	private SensorManager sensorManager;
	private Sensor gSensor;
	private Sensor oSensor;
	private Sensor mSensor;
	private float[] mGData;
	private int mAccuracy=SensorManager.SENSOR_STATUS_ACCURACY_LOW;
	private int oAccuracy=SensorManager.SENSOR_STATUS_ACCURACY_LOW;
	// 位置提供者名称，GPS设备还是网络
	private String mLocationProvider;
	private LocationManager mLocationManager;
	private static boolean locationPopup = true;
	private static boolean need_calibration = false;
	private Vibrator vibrator;
	private final int minPassTimes=70;
	private  int xPassTimes=0;
	private  int yPassTimes=0;
	private  int zPassTimes=0;
	private  int zNegativePassTimes=0;
	private long first_time_stamp = 0;
	private static final String TAG = "LocationService";
	private static final String SERVICE_NAME = "LocationService";

	private static final long MIN_TIME = 1000l;
	private static final float MIN_DISTANCE = 10f;

	private LocationManager locationManager;
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.compass_activity_layout);
		Log.e(TAG, "========================>>>>进入指南针" );

		// 背光常亮
		 getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		initResource();
		initServices();


		if (isOPen(this)){
//			Intent intent = new Intent(this, MyService.class);
//			startService(intent);
		}else {
			Toast.makeText(CompassActivity.this, "请先打开gps", Toast.LENGTH_SHORT).show();
//			Intent intent = new Intent(
//					Settings.ACTION_LOCATION_SOURCE_SETTINGS);
//			startActivityForResult(intent, 0);
		}
//		Intent intent = new Intent(this, MyService.class);
//		startService(intent);

		locationManager = (LocationManager) getSystemService(LOCATION_SERVICE);
		if (locationManager.getProvider(LocationManager.NETWORK_PROVIDER) != null || locationManager.getProvider(LocationManager.GPS_PROVIDER) != null) {
			Log.i(TAG, "正在定位");
			/*
			 * 进行定位
			 * provider:用于定位的locationProvider字符串
			 * minTime:时间更新间隔，单位：ms
			 * minDistance:位置刷新距离，单位：m
			 * listener:用于定位更新的监听者locationListener
			 */
			locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, MIN_TIME, MIN_DISTANCE, this);
		} else {
			Log.i(TAG, "无法定位");
			//无法定位：1、提示用户打开定位服务；2、跳转到设置界面
			Toast.makeText(this, "无法定位，请打开定位服务", Toast.LENGTH_SHORT).show();
			Intent i = new Intent();
			i.setAction(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
			startActivity(i);
		}



	}



	/**
	 * void initResource()
	 * 初始化资源
	 */
	private void initResource() {
		longitude_degree = (TextView) findViewById(R.id.longitude);
		longitude_section = (TextView) findViewById(R.id.longitude_text);
		latitude_degree = (TextView) findViewById(R.id.latitude);
		latitude_section = (TextView) findViewById(R.id.latitude_text);

		img_compass_dial = (ImageView) findViewById(R.id.compass_dial);
		img_compass_pointer = (ImageView) findViewById(R.id.compass_pointer);
		cur_degree = (TextView) findViewById(R.id.compass_degree);
		cali_text = (TextView) findViewById(R.id.compass_cali);
		cur_direction = (TextView) findViewById(R.id.compass_direction);
		img_compass_bg = (RelativeLayout) findViewById(R.id.compass_background);
	}


	/**
	 * void initServices()
	 * 获取相关服务
	 */
	private void initServices() {
		// 获取传感器服务
		sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
		// 获取位置服务
		mLocationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
		// 获取振动服务
		vibrator = (Vibrator) getSystemService(Service.VIBRATOR_SERVICE);
	}

	/**
	 * getTheBestLocationServiceProvider()
	 * 得到当前最优位置获取方式
	 */
	private void getTheBestLocationServiceProvider() {

		// 条件对象，即指定条件过滤获得LocationProvider
		Criteria criteria = new Criteria();
		// 较高精度
		criteria.setAccuracy(Criteria.ACCURACY_FINE);
		// 是否需要高度信息
		criteria.setAltitudeRequired(false);
		// 是否需要方向信息
		criteria.setBearingRequired(false);
		// 是否产生费用
		criteria.setCostAllowed(true);
		// 设置低电耗
		criteria.setPowerRequirement(Criteria.POWER_LOW);
		// 根据手机当前的设置获取最优的Provider
		mLocationProvider = mLocationManager.getBestProvider(criteria, true);
//		Log.e(TAG, "mLocationProvider=="+mLocationProvider);
	}


	/**
	 * boolean isLocationServiceDisable()
	 * 判断当前是否有开启GPS或网络定位的位置服务，没有则返回true
	 */
	private boolean isLocationServiceDisable() {
		if (mLocationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
				|| mLocationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER)) {
			return false;
		} else {
			return true;
		}
	}


	/**
	 * displayAlertDialogForOpenLocationService()
	 * 弹出提示框提醒用户开启位置服务
	 */
	private void displayAlertDialogForOpenLocationService() {
		new AlertDialog.Builder(this).setTitle(R.string.allow_location_service)
				.setIcon(android.R.drawable.ic_dialog_alert)
				.setMessage(R.string.allow_location_service_describe)
				.setPositiveButton(android.R.string.ok, mAlertDlgOk)
				.setNegativeButton(android.R.string.no, null).show();
	}



	private DialogInterface.OnClickListener mAlertDlgOk = new DialogInterface.OnClickListener() {
		public void onClick(DialogInterface v, int which) {
			Intent intent = new Intent(Intent.ACTION_MAIN);
			intent.setComponent(new ComponentName("com.android.settings",
					"com.android.settings.Settings$LocationSettingsActivity"));
			startActivity(intent);
		}
	};




	/**
	 * updateLocation(Location location)
	 * @param location 位置信息
	 * 更新位置显示
	 */
	private void updateLocation(Location location) {
		if (location != null) {
			lastLatitude = location.getLatitude();
			lastLongitude = location.getLongitude();
			if (lastLongitude > 0) {
				longitude_section.setText(R.string.east_longitude);
				longitude_degree.setText(getLocationString(lastLongitude));
			} else {
				longitude_section.setText(R.string.west_longitude);
				longitude_degree.setText(getLocationString(-lastLongitude));
			}
			if (lastLatitude > 0) {
				latitude_section.setText(R.string.north_latitude);
				latitude_degree.setText(getLocationString(lastLatitude));
			} else {
				latitude_section.setText(R.string.south_latitude);
				latitude_degree.setText(getLocationString(-lastLatitude));
			}
		} else if ((lastLatitude != 0) && (lastLongitude != 0)) {
			// 当mLocationProvider发生变化时，默认使用前一次获得的值
			if (lastLongitude > 0) {
				longitude_section.setText(R.string.east_longitude);
				longitude_degree.setText(getLocationString(lastLongitude));
			} else {
				longitude_section.setText(R.string.west_longitude);
				longitude_degree.setText(getLocationString(-lastLongitude));
			}

			if (lastLatitude > 0) {
				latitude_section.setText(R.string.north_latitude);
				latitude_degree.setText(getLocationString(lastLatitude));
			} else {
				latitude_section.setText(R.string.south_latitude);
				latitude_degree.setText(getLocationString(-lastLatitude));
			}
		} else {
			// Log.e(TAG, "Location == null");
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		registerCompassListener();

		calibrationUIchange(need_calibration);

		//监听用戶在通知栏中对位置服务的状态更改
		IntentFilter filter = new IntentFilter();
		filter.addAction(LocationManager.PROVIDERS_CHANGED_ACTION);
		registerReceiver(mReceiver, filter);
		first_time_stamp = SystemClock.elapsedRealtime();
	}

	private void calibrationUIchange(boolean calibrationNeed) {
		// TODO Auto-generated method stub
		if (calibrationNeed) {
			img_compass_bg.getBackground().setAlpha(160);
			img_compass_dial.setImageResource(R.drawable.img_compass_dial_cali);
			img_compass_pointer.setVisibility(View.INVISIBLE);
			cur_degree.setVisibility(View.INVISIBLE);
			cur_direction.setVisibility(View.INVISIBLE);
			cali_text.setVisibility(View.VISIBLE);
			img_compass_dial.setRotation(0);
		}else{
			img_compass_dial.setImageResource(R.drawable.img_compass_dial);
			img_compass_pointer.setVisibility(View.VISIBLE);
			img_compass_bg.getBackground().setAlpha(255);
			cur_degree.setVisibility(View.VISIBLE);
			cur_direction.setVisibility(View.VISIBLE);
			cali_text.setVisibility(View.INVISIBLE);

			if ((isLocationServiceDisable())&&locationPopup) {
				displayAlertDialogForOpenLocationService();
				locationPopup=false;
			}


		}
	}



	private BroadcastReceiver mReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();
			if ((LocationManager.PROVIDERS_CHANGED_ACTION.equals(action))) {
				//在监听到用户在通知栏中对位置服务进行变动后，更新指南针的位置服务的状态或获取方式
				registerLocationService();
			}
		}
	};



	/**
	 * void registerCompassListener()
	 * 注册指南针传感器和位置服务
	 */
	private void registerCompassListener() {

		gSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
		oSensor = sensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION);
		mSensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
		if (gSensor != null) {
			sensorManager.registerListener(this, gSensor, SensorManager.SENSOR_DELAY_GAME);
		}
		if (oSensor != null) {
			sensorManager.registerListener(this, oSensor, SensorManager.SENSOR_DELAY_GAME);
		}
		if (mSensor != null) {
			sensorManager.registerListener(this, mSensor, SensorManager.SENSOR_DELAY_GAME);
		}

		registerLocationService();
	}


	/**
	 * void registerLocationService()
	 * 注册位置获取服务
	 */
	private void registerLocationService() {

		if (isLocationServiceDisable()) {
			if (mLocationProvider != null) {
				mLocationManager.removeUpdates(mLocationListener);
				mLocationProvider = null;
//				Log.e(TAG, "mLocationManager.removeUpdates(mLocationListener)");
			}
		} else {
			getTheBestLocationServiceProvider();
			if (mLocationProvider != null) {
				// 直接获取最后一次得到的位置信息
				updateLocation(mLocationManager
						.getLastKnownLocation(mLocationProvider));
				// 8秒或者距离变化800米时更新一次地理位置
				mLocationManager.requestLocationUpdates(mLocationProvider,
						8000, 800, mLocationListener);
//				Log.e(TAG, "mLocationManager.requestLocationUpdates");
			} else {
//				Log.e(TAG, "mLocationProvider == null");
			}
		}
	}



	@Override
	protected void onPause() {
		super.onPause();
		if (gSensor != null) {
			sensorManager.unregisterListener(this, gSensor);
		}
		if (oSensor != null) {
			sensorManager.unregisterListener(this, oSensor);
		}
		if (mSensor != null) {
			sensorManager.unregisterListener(this, mSensor);
		}
		if (mLocationProvider != null) {
			mLocationManager.removeUpdates(mLocationListener);
		}
		unregisterReceiver(mReceiver);

	}


	@Override
	public void onSensorChanged(SensorEvent event) {

		int sensorType = event.sensor.getType();
		/*konka-Compass debug-chinn-2-151105-start*/
		int degree;
		/*konka-Compass debug-chinn-2-151105-start*/
		switch (sensorType) {
			case Sensor.TYPE_ORIENTATION:
				if (!need_calibration) {
					Log.i("chinn", "onSensorChanged: event.values[0] =  "+event.values[0]);
		/*konka-Compass debug-chinn-2-151105-start*/
					degree =((int) event.values[0])%360;

					img_compass_dial.setRotation(-degree);
					cur_degree.setText(String.valueOf(degree)
							+ getString(R.string.character_degree));
					setDirection(degree);
				}
				//oSensor accuracy的值获取会比较慢，过滤掉2秒前的取值，避免值错误。
				if ((SystemClock.elapsedRealtime() - first_time_stamp) > 2000) {
					oAccuracy = event.accuracy;
				}

				break;
			case Sensor.TYPE_ACCELEROMETER:
				mGData = event.values;
				if (need_calibration) {
					judgeCalibration();
				}
				break;
			case Sensor.TYPE_MAGNETIC_FIELD:
				mAccuracy = event.accuracy;
				if ((mAccuracy == SensorManager.SENSOR_STATUS_UNRELIABLE)
						|| (oAccuracy == SensorManager.SENSOR_STATUS_UNRELIABLE)) {
					need_calibration = true;
					calibrationUIchange(need_calibration);
				}
				break;
		}
	}

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub
	}


	/**
	 * void judgeCalibration()
	 * 校准判断算法
	 *
	 */
	private void judgeCalibration() {

		if (Math.abs(mGData[0]) >= 12) {
			xPassTimes++;
		}
		if (Math.abs(mGData[1]) >= 12) {
			yPassTimes++;
		}

		if (mGData[2] >= 12) {
			zPassTimes++;
		}
		if (mGData[2] <= -5) {
			zNegativePassTimes++;
		}


		Log.i("jude", "xPassTimes==" + xPassTimes);
		Log.i("jude", "yPassTimes==" + yPassTimes);
		Log.i("jude", "zPassTimes==" + zPassTimes);
		Log.i("jude", "zNegativePassTimes==" + zNegativePassTimes);
		Log.i("jude", "orientationAccuracy==" + mAccuracy);
		Log.i("jude", "oSensor oAccuracy==" + oAccuracy);
		if ((xPassTimes > minPassTimes)
				&& (yPassTimes > minPassTimes)
				&& (zPassTimes > minPassTimes)
				&& (zNegativePassTimes > minPassTimes)
				&& (mAccuracy >= SensorManager.SENSOR_STATUS_ACCURACY_LOW)
				&& (oAccuracy >= SensorManager.SENSOR_STATUS_ACCURACY_LOW)) {
			need_calibration = false;
			// 成功校准后振动提醒
			vibrator.vibrate(500);
			//calibrationComplete();
			calibrationUIchange(need_calibration);


//			String a="x="+xPassTimes+"y="+yPassTimes+"z="+zPassTimes+"zN="+zNegativePassTimes;
//			Toast toast = Toast.makeText(this,
//					a, Toast.LENGTH_SHORT);
//			toast.show();


			xPassTimes=0;
			yPassTimes=0;
			zPassTimes=0;
			zNegativePassTimes=0;



		}
	}


//	/**
//	 * void calibrationComplete()
//	 * 校准完成后进入指南针界面需切换UI视图，并判断是否开启了位置服务。
//	 */
//	private void calibrationComplete() {
//
//		img_compass_dial.setImageResource(R.drawable.img_compass_dial);
//		img_compass_pointer.setVisibility(View.VISIBLE);
//		img_compass_bg.getBackground().setAlpha(255);
//		cur_degree.setVisibility(View.VISIBLE);
//		cur_direction.setVisibility(View.VISIBLE);
//		cali_text.setVisibility(View.INVISIBLE);
////		Log.i("ACCELEROMETER", "calibrationComplete");
//		//如未开启位置服务，则提醒用户开启。
//		if (isLocationServiceDisable()) {
////			Log.i("ACCELEROMETER", "displayAlertDialogForOpenLocationService");
//			displayAlertDialogForOpenLocationService();
//		}
//
//	}

	LocationListener mLocationListener = new LocationListener() {

		// Provider的状态在可用、暂时不可用和无服务三个状态直接切换时触发此函数
		@Override
		public void onStatusChanged(String provider, int status, Bundle extras) {
			if (status != LocationProvider.OUT_OF_SERVICE) {
				updateLocation(mLocationManager.getLastKnownLocation(mLocationProvider));
			} else {
//				Log.e(TAG, "LocationStatus == LocationProvider.OUT_OF_SERVICE");
			}
		}
		// Provider被enable时触发此函数，比如GPS被打开
		@Override
		public void onProviderEnabled(String provider) {
		}
		// Provider被disable时触发此函数，比如GPS被关闭
		@Override
		public void onProviderDisabled(String provider) {
		}
		// 当坐标改变时触发此函数，如果Provider传进相同的坐标，它就不会被触发
		@Override
		public void onLocationChanged(Location location) {
			if (location != null) {
//				Log.e(TAG, "Location changed : Lat== " + location.getLatitude()+ " Lng== " + location.getLongitude());
				updateLocation(location);
			}
		}
	};


	/**
	 * String getLocationString(double input)
	 * 把经纬度转换成度分秒显示
	 * @param input 经纬度原始信息
	 * @return A°B′C″格式的经纬度信息
	 */
	private String getLocationString(double input) {
		int du = (int) input;
		int fen = (((int) ((input - du) * 3600))) / 60;
		int miao = (((int) ((input - du) * 3600))) % 60;
		return String.valueOf(du) + getString(R.string.character_degree)
				+ String.valueOf(fen) + getString(R.string.character_fen)
				+ String.valueOf(miao) + getString(R.string.character_miao);
	}


	/**
	 * void setDirection(int degree)
	 * 东/南/西/北等当前朝向信息显示
	 * @param degree 指南针表盘旋转的角度
	 */
	private void setDirection(int degree) {
		Log.i("chinn", "setDirection: degree = "+degree);
		if ((degree >= 350) || (degree < 10)) {
			cur_direction.setText(R.string.direct_north);
		} else if ((degree >= 10) && (degree < 80)) {
			cur_direction.setText(R.string.direct_north_east);
		} else if ((degree >= 80) && (degree < 100)) {
			cur_direction.setText(R.string.direct_east);
		} else if ((degree >= 100) && (degree < 170)) {
			cur_direction.setText(R.string.direct_south_east);
		} else if ((degree >= 170) && (degree < 190)) {
			cur_direction.setText(R.string.direct_south);
		} else if ((degree >= 190) && (degree < 260)) {
			cur_direction.setText(R.string.direct_south_west);
		} else if ((degree >= 260) && (degree < 280)) {
			cur_direction.setText(R.string.direct_west);
		} else if ((degree >= 280) && (degree < 350)) {
			cur_direction.setText(R.string.direct_north_west);
		}
	}




//	private boolean isServiceRunning() {
//		ActivityManager manager = (ActivityManager) getSystemService(ACTIVITY_SERVICE);
//		for (ActivityManager.RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
//			if ("com.example.getlocation.demo.MyService".equals(service.service.getClassName())) {
//				return true;
//			}
//		}
//		return false;
//	}
	/**
	 * 判断GPS是否开启，GPS或者AGPS开启一个就认为是开启的
	 * @param context
	 * @return true 表示开启
	 */
	public static final boolean isOPen(final Context context) {
		LocationManager locationManager
				= (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
		// 通过GPS卫星定位，定位级别可以精确到街（通过24颗卫星定位，在室外和空旷的地方定位准确、速度快）
		boolean gps = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
		// 通过WLAN或移动网络(3G/2G)确定的位置（也称作AGPS，辅助GPS定位。主要用于在室内或遮盖物（建筑群或茂密的深林等）密集的地方定位）
		boolean network = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
		if (gps || network) {
			return true;
		}

		return false;
	}



	@Override
	public void onLocationChanged(Location location) {

		//得到纬度
		double latitude = location.getLatitude();
		//	double latitude = 22.733726;
		Log.i(TAG, "纬度 =" + latitude);
		//得到经度
		double longitude = location.getLongitude();
		//	double longitude = 113.910660;
		Log.i(TAG, "经度 =" + longitude);

		List<Address> locationList = getLocationList(latitude, longitude);
		if (!locationList.isEmpty()) {
			Address address = locationList.get(0);
			Log.i(TAG, "address =" + address);
			String countryName = address.getCountryName();
			Log.i(TAG, "countryName = " + countryName);
			String countryCode = address.getCountryCode();
			Log.i(TAG, "countryCode = " + countryCode);
			String locality = address.getLocality();
			Log.i(TAG, "locality = " + locality);
			StringBuilder builder = new StringBuilder();
			for (int i = 0; address.getAddressLine(i) != null; i++) {
				String addressLine = address.getAddressLine(i);
				Log.i(TAG, "addressLine = " + addressLine);
			}

			builder.append(address.getAddressLine(0)).append(address.getAddressLine(1));
			builder.append(address.getFeatureName());
			latitude_section.setText("当前位置:"+builder.toString());
			latitude_degree.setText("经度:"+location.getLatitude());
			longitude_degree.setText(" 纬度:"+location.getLongitude());


		}


		// 通知Activity
//		Intent intent = new Intent();
//		intent.setAction(Common.LOCATION_ACTION);
//		intent.putExtra(Common.LOCATION, location.toString());
// 		sendBroadcast(intent);

	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {

	}

	@Override
	public void onProviderEnabled(String provider) {

	}

	@Override
	public void onProviderDisabled(String provider) {

	}
	private List<Address> getLocationList(double latitude, double longitude) {
		Log.i(TAG, "getLocationList");
		Geocoder gc = new Geocoder(this, Locale.getDefault());
		List<Address> locationList = null;
		try {
			locationList = gc.getFromLocation(latitude, longitude, 1);
		} catch (IOException e) {
			e.printStackTrace();
		}
		return locationList;
	}

	@Override
	public void onBackPressed() {
		super.onBackPressed();
		finish();
		Log.e(TAG, "========================>>>>退出指南针" );

	}
}

