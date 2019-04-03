package gbpe.policeass.utils.xmlutil;

import android.graphics.Bitmap;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import java.util.Set;

import gbpe.policeass.Setup.Setup;

/**
 * =====================================================================================================
 * 
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/4/18 10:34
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/4/18 10:34
 * 
 * 描述：SAX方式解析XML(逐行扫描)
 *   优点：不用实现调入整个文档，占用资源少。尤其在嵌入式环境中，如android，极力推荐使用SAX解析。
 　  缺点：不像DOM解析一样将文档长期驻留在内存中，数据不是持久的。如果事件过后没有保存数据，数据就会丢失。
 * ======================================================================================================
 */



public class MySaxHandler extends DefaultHandler{
    private String content;
    public static final String Resolution = "Resolution";
    public static final String Quality = "Quality";
    public static final String PictureSize = "PictureSize";
    public static final String ContinuousShooting = "ContinuousShooting";
    public static final String SegmentedVideo = "SegmentedVideo";
    public static final String Prerecord = "Prerecord";
    public static final String VideoDelay = "VideoDelay";
    public static final String ExposureCompensation = "ExposureCompensation";
    public static final String MotionDetection = "MotionDetection";
    public static final String InfraRed = "InfraRed";
    public static final String IndicatorLight = "IndicatorLight";
    public static final String GPS = "GPS";
    public static final String USBMode = "USBMode";
    public static final String FileView = "FileView";
    public static final String Volume = "Volume";
    public static final String KeyTone = "KeyTone";
    public static final String AutoTurnOffScreen = "AutoTurnOffScreen";
    public static final String AutomaticShutdown = "AutomaticShutdown";
    public static final String Language = "Language";
    public static final String DefaultSetting = "DefaultSetting";
    public MySaxHandler() {

    }

    @Override
    public void startDocument() throws SAXException {
        super.startDocument();
    }

    @Override
    public void endDocument() throws SAXException {
        super.endDocument();
    }

    @Override
    public void characters(char[] ch, int start, int length) throws SAXException {
        super.characters(ch, start, length);
        content = new String(ch, start, length);
    }

    @Override
    public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
        super.startElement(uri, localName, qName, attributes);
        if("E12Parm".equals(localName)){

         }
    }

    @Override
    public void endElement(String uri, String localName, String qName) throws SAXException {
        super.endElement(uri, localName, qName);
        switch (localName) {
            case Resolution:
                break;
            case Quality:
               // Setup.PictureQuality = content;
                break;
            case PictureSize:
                break;
            case ContinuousShooting:
                break;
            case SegmentedVideo:
                break;
            case Prerecord:
                Setup.videoPrerecordTime = Integer.parseInt(content);
                break;
            case VideoDelay:
                Setup.VideoDelay = Integer.parseInt(content);
                break;
            case ExposureCompensation:
                break;
            case MotionDetection:
                break;
            case InfraRed:
                break;
            case IndicatorLight:
                break;
            case GPS:
                Setup.GPSFuntion = Boolean.parseBoolean(content);
                break;
            case USBMode:
                break;
            case FileView:
              //  Setup.FileView =String.p(content);
                break;
            case Volume:
                break;
            case KeyTone:
                Setup.KeyTone = Boolean.parseBoolean(content);
                break;
            case AutoTurnOffScreen:
                Setup.AutoTurnOffScreen = Integer.parseInt(content);
                break;
            case AutomaticShutdown:
                Setup.AutomaticShutdown = Long.parseLong(content);
                break;
            case Language:
               // Setup.LanguageZfy = content;
                break;
            case DefaultSetting:
                break;
            default:
                break;
        }

    }
}
