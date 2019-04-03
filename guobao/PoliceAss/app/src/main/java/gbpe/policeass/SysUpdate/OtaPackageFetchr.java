package gbpe.policeass.SysUpdate;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

import gbpe.policeass.utils.AppLog;

public class OtaPackageFetchr {

    private static final String TAG = "OtaPackageFetchr";
    private static final String XML_ROMID = "romid";
    private static final String XML_CODE = "versionCode";
    private static final String XML_NAME = "versionName";
    private static final String XML_URL = "url";
    private static final String XML_DESC = "versionDesc";


    private byte[] getUrlBytes(String urlSpec) throws IOException{

        URL url = new URL(urlSpec);
        HttpURLConnection connection = (HttpURLConnection)url.openConnection();

        try {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            InputStream in = connection.getInputStream();

            if (connection.getResponseCode() != HttpURLConnection.HTTP_OK) {
                return null;
            }

            int bytesRead = 0;
            byte[] buffer = new byte[1024];
            while ((bytesRead = in.read(buffer)) > 0) {
                out.write(buffer, 0, bytesRead);
            }

            out.close();
            return out.toByteArray();
        } finally {
            connection.disconnect();
        }

    }

    public String getUrl(String urlSpec) throws IOException {
        return new String(getUrlBytes(urlSpec));
    }

    public ArrayList<OtaPackageItem> fetchItems(String ip, int port, String romid) {

        ArrayList<OtaPackageItem> items = new ArrayList<OtaPackageItem>();

        StringBuilder sb = new StringBuilder("http://");
        sb.append(ip);
        sb.append(":");
        sb.append(port);
        sb.append("/"+"api/client/romupdate.php?romid=");
        sb.append(romid);
        try {
            String url = sb.toString();
            AppLog.i (TAG, "url: "+url);
            String result = new OtaPackageFetchr().getUrl(sb.toString());
            AppLog.i(TAG,"get result "+result);
            parseJsonItems(items,result);
            //XmlPullParserFactory factory = XmlPullParserFactory.newInstance();
            //XmlPullParser parser = factory.newPullParser();
            //parser.setInput(new StringReader(result));
            //parseItems(items, parser);
        } catch (JSONException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return items;

    }

    private void parseJsonItems(ArrayList<OtaPackageItem>items, String json)
        throws JSONException{

        JSONObject jsonObject = new JSONObject(json);

        if (jsonObject != null) {
            String id = jsonObject.optString(XML_ROMID);
            String code = jsonObject.optString(XML_CODE);
            String name = jsonObject.optString(XML_NAME);
            String url = jsonObject.optString(XML_URL);
            String desc = jsonObject.optString(XML_DESC);

            OtaPackageItem otaPackageItem= new OtaPackageItem();
            otaPackageItem.setRomid(id);
            otaPackageItem.setVersionCode(code);
            otaPackageItem.setVersionName(name);
            otaPackageItem.setUrl(url);
            otaPackageItem.setVersionDesc(desc);
            items.add(otaPackageItem);
        }
    }

}
