




import java.io.FileOutputStream;

import org.jdom2.Attribute;
import org.jdom2.Comment;
import org.jdom2.Document;
import org.jdom2.Element;
import org.jdom2.output.Format;
import org.jdom2.output.XMLOutputter;

/**
 * 使用JDOM构造一个XML文档，并输出
 * 
 */
public class testxml
{

    public static void main(String[] args) throws Exception
    {
        // 文档
        Document document = new Document();

        // 生成一个元素
        Element root = new Element("inputxml");

        // 将生成的元素加入文档：根元素
        document.addContent(root);

        // 给结点加上注释
        Comment comment = new Comment("This is my comments");
        root.addContent(comment);

        // 加上子元素1
        Element e = new Element("isOK");
        // 加属性
       
        e.setText("isok");
        root.addContent(e);

        // 加子元素2
        Element e2 = new Element("world");
        Attribute attribute = new Attribute("testAttr", "attr Value");
        e2.setAttribute(attribute);// set方法会返回元素本身（方法链method chain style）
        root.addContent(e2);

        e2.addContent(new Element("subElement").setAttribute("a", "aValue")
                .setAttribute("x", "xValue").setAttribute("y", "yValue")
                .setText("textContent"));

        // 格式化
        Format format = Format.getPrettyFormat();
        // Format.getRawFormat()方法，通常用于XML数据的网络传输，因为这种格式会去掉所有不必要的空白，因此能够减少数据量

        // 可以自己设定一些format的属性
        format.setIndent("    ");// 把缩进设为四个空格（默认为两个空格）

        // 输出
        XMLOutputter out = new XMLOutputter(format);
        out.output(document, new FileOutputStream("jdom.xml"));// 可在当前项目路径下找到

    }
}