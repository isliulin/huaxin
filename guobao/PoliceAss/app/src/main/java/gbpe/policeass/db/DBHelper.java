package gbpe.policeass.db;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.database.sqlite.SQLiteOpenHelper;

import java.io.File;

public class DBHelper extends SQLiteOpenHelper {
	private final static int VERSION = 1;
	private final static String DB_NAME = "phones.db";
	private final static String TABLE_NAME = "phone";
	//private final static String CREATE_TBL = "create table phone(_id integer primary key autoincrement, name text, sex text, number text, desc text)";
	private final static String CREATE_TABLE = "create table phone(_id integer primary key autoincrement, startTime text, stopTime text," +
			" size text, path text, latitude text, longitude text)";
	private SQLiteDatabase db;

	//SQLiteOpenHelper子类必须要的一个构造函数
	public DBHelper(Context context, String name, CursorFactory factory, int version) {
		//必须通过super 调用父类的构造函数
		super(context, name, factory, version);
	}

	//数据库的构造函数，传递三个参数的
	public DBHelper(Context context, String name, int version){

		this(context, name, null, version);
	}

	//数据库的构造函数，传递一个参数的， 数据库名字和版本号都写死了
	public DBHelper(Context context){
		this(context, DB_NAME, null, VERSION);
	}


	// 回调函数，第一次创建时才会调用此函数，创建一个数据库
	@Override
	public void onCreate(SQLiteDatabase db) {
		this.db = db;
		System.out.println("Create Database");
		//db.execSQL(CREATE_TBL);
		db.execSQL(CREATE_TABLE);
	}

	//回调函数，当你构造DBHelper的传递的Version与之前的Version调用此函数
	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
		System.out.println("update Database");

	}

	//插入方法
	public void insert(ContentValues values){
		//获取SQLiteDatabase实例
		SQLiteDatabase db = getWritableDatabase();
		//插入数据库中
		db.insert(TABLE_NAME, null, values);
		db.close();
	}

	//查询方法
	public Cursor query(){
		SQLiteDatabase db = getReadableDatabase();
		//获取Cursor
		Cursor c = db.query(TABLE_NAME, null, null, null, null, null, null, null);
		return c;

	}

	//根据唯一标识_id  来删除数据
	public void delete(int id){
		SQLiteDatabase db = getWritableDatabase();
		db.delete(TABLE_NAME, "_id=?", new String[]{String.valueOf(id)});
	}


	//更新数据库的内容
	public void update(ContentValues values, String whereClause, String[]whereArgs){
		SQLiteDatabase db = getWritableDatabase();
		db.update(TABLE_NAME, values, whereClause, whereArgs);
	}

	//关闭数据库
	public void close(){
		if(db != null){
			db.close();
		}
	}

	//进入系统首先查询数据库，如果该数据库中的文件不存在，则从数据库中删除该文件



	public static void quary(DBHelper dbHelper) {

		Cursor cursor = dbHelper.query();
		if (cursor != null) {
			while (cursor.moveToNext()) {
				int id = cursor.getInt(cursor.getColumnIndex("_id"));
				String startTime = cursor.getString(cursor.getColumnIndex("startTime"));
				String stopTime = cursor.getString(cursor.getColumnIndex("stopTime"));
				String size = cursor.getString(cursor.getColumnIndex("size"));
				String path = cursor.getString(cursor.getColumnIndex("path"));
				String latitude = cursor.getString(cursor.getColumnIndex("latitude"));
				String longitude = cursor.getString(cursor.getColumnIndex("longitude"));
				System.out.println("---->>"+"_id:"+id+" startTime:" + startTime + "　stopTime:" + stopTime + " " +
						"size:" + size + "　path:" + path + " latitude:" + latitude +
						" longtitud:" + longitude);
				System.out.println("--------->>>"+new File(path).exists());
				if (!new File(path).exists()){
				    dbHelper.delete(id);
				}
			}
			cursor.close();
		}
	}
}
