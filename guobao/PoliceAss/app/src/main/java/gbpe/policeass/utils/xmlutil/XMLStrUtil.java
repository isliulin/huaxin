package gbpe.policeass.utils.xmlutil;

import android.content.Context;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

import javax.xml.parsers.DocumentBuilderFactory;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.List;

import gbpe.policeass.Setup.Setup;
import gbpe.policeass.fragments.SettingFragment;
import gbpe.policeass.utils.SettingUtils;
import gbpe.policeass.utils.StringUtil;



public class XMLStrUtil {

    public static List<E12DeviceParam> getParaValue(String xmlstr) throws Exception {
        List<E12DeviceParam> list = new ArrayList<>();
        InputSource is = new InputSource(new StringReader(xmlstr));
        Document doc = DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(is); //doc容器
        String paraname = "E12Parm";
        // 按文档顺序返回包含在文档中且具有给定标记名称的所有Element的NodeList
        NodeList nl = doc.getElementsByTagName(paraname);
        int beanNum = nl.getLength(); //获取NodeList数目
        for(int i=0;i<beanNum;i++) {
            Node n = nl.item(i); //NodeList中的某一个节点
            E12DeviceParam params = new E12DeviceParam();
            params.setResolution(n.getChildNodes().item(0).getTextContent());
            params.setVideoQuality(n.getChildNodes().item(1).getTextContent());
            params.setPictureSize(n.getChildNodes().item(2).getTextContent());
            params.setContinuousShooting(n.getChildNodes().item(3).getTextContent());
            params.setSegmentedVideo(n.getChildNodes().item(4).getTextContent());
            params.setPrerecord(n.getChildNodes().item(5).getTextContent());
            params.setVideoDelay(n.getChildNodes().item(6).getTextContent());
            params.setExposureCompensation(n.getChildNodes().item(7).getTextContent());
            params.setMotionDetection(n.getChildNodes().item(8).getTextContent());
            params.setInfraRed(n.getChildNodes().item(9).getTextContent());
            params.setIndicatorLight(n.getChildNodes().item(10).getTextContent());
            params.setGPS(n.getChildNodes().item(11).getTextContent());
            params.setUSBMode(n.getChildNodes().item(12).getTextContent());
            params.setFileView(n.getChildNodes().item(13).getTextContent());
            params.setVolume(n.getChildNodes().item(14).getTextContent());
            params.setKeyTone(n.getChildNodes().item(15).getTextContent());
            params.setAutoTurnOffScreen(n.getChildNodes().item(16).getTextContent());
            params.setAutomaticShutdown(n.getChildNodes().item(17).getTextContent());
            params.setLanguage(n.getChildNodes().item(18).getTextContent());
            params.setDefaultSetting(n.getChildNodes().item(19).getTextContent());
            params.setVideoBitrate(n.getChildNodes().item(20).getTextContent());
            params.setAudioOutputFormat(n.getChildNodes().item(21).getTextContent());
            params.setVideoOutputFormat(n.getChildNodes().item(22).getTextContent());
            params.setPicOutputFormat(n.getChildNodes().item(23).getTextContent());
            params.setPicQuality(n.getChildNodes().item(24).getTextContent());
            params.setAudioSimpleRate(n.getChildNodes().item(25).getTextContent());
            params.setVideoMaxDuration(n.getChildNodes().item(26).getTextContent());
            params.setAudioMaxDuration(n.getChildNodes().item(27).getTextContent());
            params.setPreviewSize(n.getChildNodes().item(28).getTextContent());
            params.setVibrate(n.getChildNodes().item(29).getTextContent());
            params.setSos(n.getChildNodes().item(30).getTextContent());
             list.add(params);
        }

        return list;
    }

    public static String getXMLString(){
        StringBuilder builder = new StringBuilder();
        builder.append("<E12Parm>");
        builder.append("<Resolution>").append("848x480P60").append("</Resolution>");
        builder.append("<Quality>").append("hello").append("</Quality>");
        builder.append("<PictureSize>").append("5.020000").append("</PictureSize>");
        builder.append("<ContinuousShooting>").append("2.585290e+001").append("</ContinuousShooting>");
        builder.append("<SegmentedVideo>").append("1834557").append("</SegmentedVideo>");
        builder.append("<Prerecord>").append("1").append("</Prerecord>");
        builder.append("<VideoDelay>").append("100000000000000").append("</VideoDelay>");
        builder.append("<ExposureCompensation>").append("hello").append("</ExposureCompensation>");
        builder.append("<MotionDetection>").append("1").append("</MotionDetection>");
        builder.append("<InfraRed>").append("1").append("</InfraRed>");
        builder.append("<IndicatorLight>").append("1").append("</IndicatorLight>");
        builder.append("<GPS>").append("1").append("</GPS>");
        builder.append("<USBMode>").append("1").append("</USBMode>");
        builder.append("<FileView>").append("1").append("</FileView>");
        builder.append("<Volume>").append("1").append("</Volume>");
        builder.append("<KeyTone>").append("1").append("</KeyTone>");
        builder.append("<AutoTurnOffScreen>").append("1").append("</AutoTurnOffScreen>");
        builder.append("<AutomaticShutdown>").append("1").append("</AutomaticShutdown>");
        builder.append("<Language>").append("1").append("</Language>");
        builder.append("<DefaultSetting>").append("1").append("</DefaultSetting>");
        builder.append("</E12Parm>");
        return builder.toString();
    }


    public static String getXMLString2(Context context){
        StringBuilder builder = new StringBuilder();

        builder.append("<E12Parm>");
        builder.append("<Resolution>").append(SettingUtils.getStrValues(context,"video_size")).append("</Resolution>");

        builder.append("<VideoQuality>").append(SettingUtils.getStrValues(context,"video_quality")).append("</VideoQuality>");
        builder.append("<PictureSize>").append(SettingUtils.getStrValues(context,"picture_size")).append("</PictureSize>");
        builder.append("<ContinuousShooting>").append(SettingUtils.getStrValues(context,"jpeg_continous")).append("</ContinuousShooting>");

        builder.append("<SegmentedVideo>").append(SettingUtils.getStrValues(context,"video_segmented")).append("</SegmentedVideo>");
        builder.append("<Prerecord>").append(SettingUtils.getStrValues(context,"video_prerecord")).append("</Prerecord>");
        builder.append("<VideoDelay>").append(SettingUtils.getStrValues(context,"video_delay")).append("</VideoDelay>");
        builder.append("<ExposureCompensation>").append(SettingUtils.getStrValues(context,"exposure_compensation")).append("</ExposureCompensation>");
     //   builder.append("<MotionDetection>").append(StringUtil.boolToChinese(SettingUtils.getBoolenValues(context,"video_motiondetection"))).append("</MotionDetection>");


        builder.append("<InfraRed>").append(SettingUtils.getStrValues(context,"InfraRed")).append("</InfraRed>");
        builder.append("<IndicatorLight>").append(StringUtil.boolToChinese(Setup.IndicatorLight)).append("</IndicatorLight>");

        builder.append("<GPS>").append(StringUtil.boolToChinese(SettingUtils.getBoolenValues(context, SettingFragment.KEY_PREF_GPS_DATA))).append("</GPS>");
        builder.append("<USBMode>").append(SettingUtils.getStrValues(context,"udisk")).append("</USBMode>");
        builder.append("<FileView>").append(SettingUtils.getStrValues(context,SettingFragment.KEY_FILE_VIEW_TYPE)).append("</FileView>");
        builder.append("<Volume>").append("1").append("</Volume>");
        builder.append("<KeyTone>").append(StringUtil.boolToChinese(SettingUtils.getBoolenValues(context,SettingFragment.KEY_KEY_TONE))).append("</KeyTone>");


        builder.append("<AutoTurnOffScreen>").append("1").append("</AutoTurnOffScreen>");
        builder.append("<AutomaticShutdown>").append("1").append("</AutomaticShutdown>");

        builder.append("<Language>").append(SettingUtils.getStrValues(context,SettingFragment.KEY_SYSTEM_LANGUAGE)).append("</Language>");
        builder.append("<DefaultSetting>").append("关").append("</DefaultSetting>");

        builder.append("<VideoBitrate>").append(SettingUtils.getStrValues(context,"video_rate")).append("</VideoBitrate>");
        builder.append("<AudioOutPutFormat>").append(SettingUtils.getStrValues(context,"audio_outputformat")).append("</AudioOutPutFormat>");
        builder.append("<VideoOutPutFormat>").append(SettingUtils.getStrValues(context,"video_outputformat")).append("</VideoOutPutFormat>");
        builder.append("<PicOutPutFormat>").append(SettingUtils.getStrValues(context,"jpeg_outputformat")).append("</PicOutPutFormat>");
        builder.append("<PicQuality>").append(SettingUtils.getStrValues(context,"jpeg_quality")).append("</PicQuality>");
        builder.append("<AudioSampleRate>").append(SettingUtils.getStrValues(context,"audio_samplerate")).append("</AudioSampleRate>");
        builder.append("<VideoMaxDuration>").append(SettingUtils.getStrValues(context,"video_maxduration")).append("</VideoMaxDuration>");
        builder.append("<AudioMaxDuration>").append(SettingUtils.getStrValues(context,"audio_maxduration")).append("</AudioMaxDuration>");
        builder.append("<PreviewSize>").append(SettingUtils.getStrValues(context,"preview_size")).append("</PreviewSize>");
        builder.append("<Vibrate>").append(StringUtil.boolToChinese(SettingUtils.getBoolenValues(context,"vibrate"))).append("</Vibrate>");
        builder.append("<Sos>").append(SettingUtils.getStrValues(context,"sos")).append("</Sos>");

        builder.append("</E12Parm>");

        return builder.toString();
    }
}
