package gbpe.policeass.modle;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/8/22 15:41
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/8/22 15:41
 * <p/>
 * 描述：
 * =====================================================================================
 */
public class Automatic {
    public String getMsg() {
        return msg;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }

    String msg = "";
    public Automatic(String msg) {
         this.msg = msg;
    }
}
