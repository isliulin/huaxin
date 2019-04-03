package gbpe.policeass.utils.xmlutil;

import android.util.Xml;

import org.xmlpull.v1.XmlSerializer;

import java.io.OutputStream;
import java.util.List;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.modle.LocationInfo;

/**
 * =====================================================================================
 * 
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/4/18 14:26
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/4/18 14:26
 * 
 * 描述：将用户的姓名，警员编号，设备编号，视频时长，开始、结束时间以及经纬度变化的值写进xml文件
 * 中，每一个视频文件对应一个与其同名的xml文件
 * =====================================================================================
 */
public class XmlUtil {


    /**
     * 保存XML文件(视频录制完后才将这些信息保存到xml文件中)
     * @param locationInfos：经纬度信息
     * @param out：文件输出流
     * @param videoDuration:视频时长
     * @param beginTime：视频开始时间
     * @param endTime：视频结束时间
     * @throws Exception
     */
    public static void save(List<LocationInfo> locationInfos, OutputStream out,
                            String videoDuration,String beginTime,String endTime) throws Exception {

        XmlSerializer serializer = Xml.newSerializer();//使用Android提供XML工具得到序列化器
        serializer.setOutput(out, "UTF-8");//设置序列化器输出方向和编码
        serializer.startDocument("UTF-8", true);

        serializer.startTag (null, "locations");
        serializer.attribute(null, "usrName", Setup.usrName);
        serializer.attribute(null, "policeNum", Setup.policeNumber);
        serializer.attribute(null, "devNum", Setup.devNum);
        serializer.attribute(null, "videoDuration", videoDuration);
        serializer.attribute(null, "beginTime", beginTime);
        serializer.attribute(null, "endTime", endTime);

        for (LocationInfo info : locationInfos) {

            serializer.startTag(null, "location");
            serializer.attribute(null, "id", "" + info.getId());

            serializer.startTag(null, "longitude");
            serializer.text(info.getLongitude()+"");
            serializer.endTag(null, "longitude");

            serializer.startTag(null, "latitude");
            serializer.text(info.getLatitude() + "");
            serializer.endTag(null, "latitude");

            serializer.endTag(null, "location");
        }


        serializer.endTag(null, "locations");
        serializer.endDocument();

        out.flush();//以防是缓存数据
        out.close();
    }




    /**
     * 保存XML文件(视频录制完后才将这些信息保存到xml文件中)
     * @param locationInfos：经纬度信息
     * @param out：文件输出流
     * @throws Exception
     */
    public static void save2(List<String> locationInfos, OutputStream out) throws Exception {

        XmlSerializer serializer = Xml.newSerializer();//使用Android提供XML工具得到序列化器
        serializer.setOutput(out, "UTF-8");//设置序列化器输出方向和编码
        serializer.startDocument("UTF-8", true);

        serializer.startTag (null, "E12Parm");
            serializer.startTag(null, "Resolution");
            serializer.text(locationInfos.get(0));
            serializer.endTag(null, "Resolution");

            serializer.startTag(null, "VideoQuality");
            serializer.text(locationInfos.get(1));
            serializer.endTag(null, "VideoQuality");

            serializer.startTag(null, "PictureSize");
            serializer.text(locationInfos.get(2));
            serializer.endTag(null, "PictureSize");

            serializer.startTag(null, "USBMode");
            serializer.text(locationInfos.get(3));
            serializer.endTag(null, "USBMode");

        serializer.endTag(null, "E12Parm");
        serializer.endDocument();

        out.flush();//以防是缓存数据
        out.close();
    }


    public static void getZFYInfo(OutputStream out) throws Exception{
        XmlSerializer serializer = Xml.newSerializer();//使用Android提供XML工具得到序列化器
        serializer.setOutput(out, "UTF-8");//设置序列化器输出方向和编码
        serializer.startDocument("UTF-8", true);
        serializer.startTag (null, "E12Parm");


        serializer.startTag(null,"Resolution");
        serializer.text("");
        serializer.endTag(null,"Resolution");

        serializer.startTag(null,"Quality");
        serializer.text("");
        serializer.endTag(null,"Quality");

        serializer.startTag(null,"PictureSize");
        serializer.text("");
        serializer.endTag(null,"PictureSize");

        serializer.startTag(null,"ContinuousShooting");
        serializer.text("");
        serializer.endTag(null,"ContinuousShooting");

        serializer.startTag(null,"SegmentedVideo");
        serializer.text("");
        serializer.endTag(null,"SegmentedVideo");

        serializer.startTag(null,"VideoDelay");
        serializer.text("");
        serializer.endTag(null,"VideoDelay");

        serializer.startTag(null,"ExposureCompensation");
        serializer.text("");
        serializer.endTag(null,"ExposureCompensation");

        serializer.startTag(null,"MotionDetection");
        serializer.text("");
        serializer.endTag(null,"MotionDetection");

        serializer.startTag(null,"InfraRed");
        serializer.text("");
        serializer.endTag(null,"InfraRed");

        serializer.startTag(null,"IndicatorLight");
        serializer.text("");
        serializer.endTag(null,"IndicatorLight");

        serializer.startTag(null,"GPS");
        serializer.text("");
        serializer.endTag(null,"GPS");

        serializer.startTag(null,"USBMode");
        serializer.text("");
        serializer.endTag(null,"USBMode");

        serializer.startTag(null,"FileView");
        serializer.text("");
        serializer.endTag(null,"FileView");

        serializer.startTag(null,"Volume");
        serializer.text("");
        serializer.endTag(null,"Volume");

        serializer.startTag(null,"KeyTone");
        serializer.text("");
        serializer.endTag(null,"KeyTone");

        serializer.startTag(null,"AutoTurnOffScreen");
        serializer.text("");
        serializer.endTag(null,"AutoTurnOffScreen");

        serializer.startTag(null,"AutomaticShutdown");
        serializer.text("");
        serializer.endTag(null,"AutomaticShutdown");

        serializer.startTag(null,"Language");
        serializer.text("");
        serializer.endTag(null,"Language");

        serializer.startTag(null,"DefaultSetting");
        serializer.text("");
        serializer.endTag(null,"DefaultSetting");


        serializer.endTag(null, "E12Parm");


        serializer.endDocument();
        out.flush();//以防是缓存数据
        out.close();
    }






}