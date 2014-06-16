DuiVision
=========

DuiVision界面库是参考了仿PC管家程序、金山界面库、DuiEngine等多个基于DirectUI的界面库开发的。

DirectUI技术一般是指将所有的界面控件都绘制在一个窗口上，这些控件的逻辑和绘图方式都必须自己进行编写和封装，而不是使用Windows控件，所以这些控件都是无句柄的。

DirectUI技术需要解决的主要问题如下：
- 1、窗口的子类化，截获窗口的消息；
- 2、封装自己的控件，并将自己的控件绘制到该窗口上；
- 3、封装窗口的消息，并分发到自己的控件上，让自己的控件根据消息进行相应和绘制；
- 4、根据不同的行为发送自定义消息给窗口，以便程序进行调用；
- 5、一般窗口上控件的组织使用XML来描述。

通常DirectUI的界面库都采用XML配置文件+图片+控制脚本（Lua、Javascript等）的开发方式，非常类似于Web程序的开发方式，当然这里面控制脚本也可以直接使用C++代码来实现。这种开发方式可以大大提高开发效率，将程序员从繁琐的界面工作中解脱出来，并且通过美工的设计，可以使界面更美观。

DuiVision开源下载地址：https://github.com/blueantst/DuiVision

蓝蚂蚁工作室主页：[http://www.blueantstudio.net](http://www.blueantstudio.net)

作者Blog：[http://blog.csdn.net/oceanheart](http://blog.csdn.net/oceanheart)

QQ群：325880743

微信公众号：blueantstudio 或搜索 蓝蚂蚁工作室

### 一些界面截图
![主界面](http://www.blueantstudio.net/duivision/suolue/duivision_home.jpg)
![控件演示](http://www.blueantstudio.net/duivision/suolue/duivision_ctrl1.jpg)
![控件演示](http://www.blueantstudio.net/duivision/suolue/duivision_ctrl2.jpg)
![控件演示](http://www.blueantstudio.net/duivision/suolue/duivision_listctrl.jpg)
![控件演示](http://www.blueantstudio.net/duivision/suolue/duivision_treectrl.jpg)
![仿360界面](http://www.blueantstudio.net/duivision/suolue/duivision_360.jpg)
![仿QQ PC管家界面](http://www.blueantstudio.net/duivision/suolue/duivision_pcmgr.jpg)
