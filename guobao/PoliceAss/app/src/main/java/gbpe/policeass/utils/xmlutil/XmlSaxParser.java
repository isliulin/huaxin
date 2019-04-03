package gbpe.policeass.utils.xmlutil;

import android.content.Context;
import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;

import java.io.InputStream;
import java.util.List;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

/**
 * =====================================================================================
 * 
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * 
 * 作者：Administrator on 2017/4/18 10:33
 * 
 * 邮箱：xjs250@163.com
 * 
 * 创建日期：2017/4/18 10:33
 * 
 * 描述：
 * =====================================================================================
 */
public class XmlSaxParser<T extends Object> {

    public T parse(Context context, List<T> beautyList){
            try {
                InputStream is = context.getAssets().open("E12PARM.xml");
                InputSource is2 = new InputSource(is);
                //得到sax解析工厂
                SAXParserFactory spf = SAXParserFactory.newInstance();
                //得到sax解析器
                SAXParser sp = spf.newSAXParser();
                //得到XMLReader，逐行扫描从文件中读取
                XMLReader xr = sp.getXMLReader();
                MySaxHandler msh = new MySaxHandler();
                //设置setContentHandler接口
                xr.setContentHandler(msh);
                xr.parse(is2);
            } catch (Exception e) {
                e.printStackTrace();
            }

        return null;
    }
}
