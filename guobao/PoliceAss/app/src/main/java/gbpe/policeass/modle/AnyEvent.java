package gbpe.policeass.modle;

/**
 * =====================================================================================
 * <p/>
 * 版权：深圳国保警用装备制造有限公司，版权所有(c)2017
 * <p/>
 * 作者：Administrator on 2017/7/4 16:31
 * <p/>
 * 邮箱：xjs250@163.com
 * <p/>
 * 创建日期：2017/7/4 16:31
 * <p/>
 * 描述：
 * =====================================================================================
 */
/**
 * event类
 * Created by Jeff on 2016/1/12.
 */
public class AnyEvent {

    private String discribe;


    //构造函数
    public AnyEvent(String discribe) {
        this.discribe = discribe;
    }

    //set/get方法
    public void setDiscribe(String discribe) {
        this.discribe = discribe;
    }

    public String getDiscribe() {
        return discribe;
    }

}