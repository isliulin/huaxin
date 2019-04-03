package gbpe.policeass.Activity;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DecimalFormat;
import java.util.List;

import gbpe.policeass.R;
import gbpe.policeass.db.FaceRcdLab;
import gbpe.policeass.db.FaceRecord;
import gbpe.policeass.utils.AppLog;

public class FaceRecordActivity extends AppCompatActivity {

    private RecyclerView mFaceRcdView;
    private static final String TAG = "FaceRecordActivity";
    private FacesRcdAdapt mAdapter;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        AppLog.i (TAG, "onCreate()");

        setContentView(R.layout.activity_face_record);
        mFaceRcdView = findViewById(R.id.face_rcd_view);
        mFaceRcdView.setLayoutManager(new LinearLayoutManager(this));
        //mFaceRcdView.setAdapter(new FacesRcdAdapt(FaceRcdLab.getInstance(this).getRecord()),this);
        //mFaceRcdView.setAdapter(new FacesRcdAdapt(FaceRcdLab.getInstance(this).getRecord(),this));
        updateUI();

    }

    private class FacesRcdHolder extends RecyclerView.ViewHolder
            implements View.OnClickListener {

        private ImageView imageView;
        private TextView nameView;
        private TextView timeView;
        private TextView locationView;
        private FaceRecord faceRecord;

        public FacesRcdHolder(LayoutInflater inflater, ViewGroup parent) {
            super(inflater.inflate(R.layout.item_list_face_record,parent,false));
            itemView.setOnClickListener(this);

            imageView = itemView.findViewById(R.id.face_rcd_photo);
            nameView = itemView.findViewById(R.id.face_rcd_name);
            timeView = itemView.findViewById(R.id.face_rcd_time);
            locationView = itemView.findViewById(R.id.location);
        }

        public void bind(FaceRecord record) {
            faceRecord = record;
            //Bitmap bitmap = BitmapFactory.decodeFile(faceRecord.getPath());
            Bitmap bitmap = getBitmapThumbnail(faceRecord.getPath());
            imageView.setImageBitmap(bitmap);
            nameView.setText(faceRecord.getName());
            timeView.setText(faceRecord.getTimeStamp());
            DecimalFormat df = new DecimalFormat("#.00000");
            String strLoc = df.format(faceRecord.getLongitude()) + " : "+df.format(faceRecord.getLongitude());
            locationView.setText(strLoc);
        }

        @Override
        public void onClick(View v) {
            AppLog.i (TAG, "");
        }
    }

    private Bitmap getBitmapThumbnail(String path) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        // 图片属性
        options.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(path, options);

        // 图片高度
        int outHeight = options.outHeight;
        int outWidth = options.outWidth;

        AppLog.i (TAG, "原始宽度："+outWidth+", 原始高度："+outHeight);

        //options.outHeight = outHeight/10;
        //options.outWidth = outWidth/10;
        options.inSampleSize = 40;
        options.inPreferredConfig = Bitmap.Config.RGB_565;
        options.inJustDecodeBounds = false;

        Bitmap newBitmap =  BitmapFactory.decodeFile(path,options);
        AppLog.i (TAG, "采用高度: "+newBitmap.getHeight()+", 采用宽度: "+newBitmap.getWidth());

        return newBitmap;
    }


    private class FacesRcdAdapt extends RecyclerView.Adapter<FacesRcdHolder>{

        private List<FaceRecord> mFaceRecords;
        private Context mContext;

        public FacesRcdAdapt(List<FaceRecord> faceRecords, Context context){
            mFaceRecords = faceRecords;
            mContext = context;
        }

        @Override
        public FacesRcdHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            //View view = LayoutInflater.from(mContext).inflate(R.layout.item_list_face_record,null);
            //return new FacesRcdHolder(view);
            LayoutInflater layoutInflater = LayoutInflater.from(mContext);
            return new FacesRcdHolder(layoutInflater,parent);
        }

        @Override
        public void onBindViewHolder(FacesRcdHolder holder, int position) {

            FaceRecord faceRecord = mFaceRecords.get(position);
            holder.bind(faceRecord);

        }

        @Override
        public int getItemCount() {
            return mFaceRecords.size();
        }
    }

    private void updateUI() {
        //FaceRcdLab.getInstance(this).getRecord();
        FaceRcdLab faceRcdLab = FaceRcdLab.getInstance(this);
        List<FaceRecord> faceRecords = faceRcdLab.getRecords();

        mAdapter = new FacesRcdAdapt(faceRecords,this);
        mFaceRcdView.setAdapter(mAdapter);
    }

}
