DuiVision
=========

DuiVision界面库是参考了仿PC管家程序、金山界面库、DuiEngine、DuiLib等多个基于DirectUI的界面库开发的。

DirectUI技术一般是指将所有的界面控件都绘制在一个窗口上，这些控件的逻辑和绘图方式都必须自己进行编写和封装，而不是使用Windows控件，所以这些控件都是无句柄的。

DirectUI技术需要解决的主要问题如下：
- 1、窗口的子类化，截获窗口的消息；
- 2、封装自己的控件，并将自己的控件绘制到该窗口上；
- 3、封装窗口的消息，并分发到自己的控件上，让自己的控件根据消息进行响应和绘制；
- 4、根据不同的行为发送自定义消息给窗口，以便程序进行调用；
- 5、一般窗口上控件的组织使用XML来描述。

通常DirectUI的界面库都采用XML配置文件+图片+控制脚本（Lua、Javascript等）的开发方式，非常类似于Web程序的开发方式，当然这里面控制脚本也可以直接使用C++代码来实现。这种开发方式可以大大提高开发效率，将程序员从繁琐的界面工作中解脱出来，并且通过美工的设计，可以使界面更美观。

DuiVision开源下载地址：https://github.com/blueantst/DuiVision

蓝蚂蚁工作室主页：[http://www.blueantstudio.net](http://www.blueantstudio.net)

作者Blog：[http://blog.csdn.net/oceanheart](http://blog.csdn.net/oceanheart)

QQ群：325880743 (Script.NET&DuiVision社区)<a target="_blank" href="http://shang.qq.com/wpa/qunwpa?idkey=7e3a1bd187395dbc62ae950b236386bf2de8a58fe83bd71f30de142619cb7aa9"><img border="0" src="http://pub.idqqimg.com/wpa/images/group.png" alt="Script.NET&amp;DuiVision社区" title="Script.NET&amp;DuiVision社区"></a>

微信公众号：blueantstudio 或搜索 蓝蚂蚁工作室

下载DuiVision Demo程序：[20160312版本](http://www.blueantstudio.net/duivision/DuiVisionDemo-20160312.zip)

下载DuiVision 浏览器演示程序：[20160312版本](http://www.blueantstudio.net/duivision/DuiVisionExplorer-20160312.zip)

下载DuiVision IM演示程序：[20160312版本](http://www.blueantstudio.net/duivision/DuiVisionIM-20160312.zip)

下载DuiVision 仿360演示程序：[20160312版本](http://www.blueantstudio.net/duivision/DuiVision360Demo-20160312.zip)

下载DuiVision exe内嵌界面资源演示程序：[20160312版本](http://www.blueantstudio.net/duivision/DuiVisionZipEmbedDemo-20160312.zip)

DuiVision提供了VC向导用于创建应用工程，以前使用向导的安装包进行向导安装，目前推荐的向导安装方法是使用网友蜡笔小辛提供的js安装向导，首先下载github代码库到本地，然后直接点击执行DuiVision\DuiWizard\SetupWizard.js文件，会提示安装哪些VC版本的向导。这种方法安装的向导优点是每次界面库有更新不需要安装新版本的向导，只要更新了github库就可以自动使用最新的向导。注意DuiVision的目录最好不要放在有空格或中文的路径下面，否则向导可能会出现一些异常问题。

下载DuiVision开发手册：[DuiVision开发手册-20160309.pdf](http://www.blueantstudio.net/duivision/DuiVision%E5%BC%80%E5%8F%91%E6%89%8B%E5%86%8C-20160309.pdf)

## License

(The MIT License)

Copyright (c) 2013-2016 blueant studio. and other contributors

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

## 一些界面演示程序截图
![主界面](http://www.blueantstudio.net/duivision/suolue/duivision_home.jpg)
![控件演示1](http://www.blueantstudio.net/duivision/suolue/duivision_ctrl1.jpg)
![控件演示2](http://www.blueantstudio.net/duivision/suolue/duivision_ctrl2.jpg)
![控件演示3](http://www.blueantstudio.net/duivision/suolue/duivision_gridctrl.jpg)
![控件演示4](http://www.blueantstudio.net/duivision/suolue/duivision_treectrl.jpg)
![支持复杂控件的弹出窗](http://www.blueantstudio.net/duivision/suolue/duivision_popuplist.jpg)
![仿360界面](http://www.blueantstudio.net/duivision/suolue/duivision_360.jpg)
![仿QQ PC管家界面](http://www.blueantstudio.net/duivision/suolue/duivision_pcmgr.jpg)
![仿360 10.0界面](http://www.blueantstudio.net/duivision/suolue/dui360.10.jpg)
![Flash对话框](http://www.blueantstudio.net/duivision/suolue/duivision_flashdlg.jpg)
![半透明对话框](http://www.blueantstudio.net/duivision/suolue/duivision_translucentdlg.jpg)
![Web控件](http://www.blueantstudio.net/duivision/suolue/duivision_webbrowser.jpg)
![菜单](http://www.blueantstudio.net/duivision/suolue/duivision_menu.jpg)

## 浏览器演示程序截图
![浏览器主界面](http://www.blueantstudio.net/duivision/suolue/duivisionexplorer.jpg)
![浏览器设置窗口](http://www.blueantstudio.net/duivision/suolue/explorer_option.jpg)

## IM演示程序截图
![IM主界面](http://www.blueantstudio.net/duivision/suolue/duivision-im.jpg)

## VC工程向导截图
![选择工程模板](http://www.blueantstudio.net/duivision/suolue/wizard.jpg)
![向导第1步](http://www.blueantstudio.net/duivision/suolue/wizard_step1.jpg)
![向导第2步](http://www.blueantstudio.net/duivision/suolue/wizard_step2.jpg)
![向导第3步](http://www.blueantstudio.net/duivision/suolue/wizard_step3.jpg)
![向导第4步](http://www.blueantstudio.net/duivision/suolue/wizard_step4.jpg)

## 贡献者名单
DuiVision的基础代码有很多是从下面几个界面库移植的：
- 仿PC管家程序 (http://blog.csdn.net/geniusice18/article/details/16369499)
- 金山界面库
- DuiEngine
- DuiLib

其他贡献者：
- blueant  : 移植代码，搭建DuiVision框架
- 齐       : XML库移植到pugixml、XML封装库等
- 蜡笔小新 : 完善VC向导...
- water    : 提供阴影窗口等代码，协助解决一些bug，提出若干改进建议
- 风儿吹吹 : 增加了多字符集的支持，支持多字符集和Unicode两种编译方式
- 黑色泡泡糖: 增加控件的鼠标右键事件处理代码
- LieberVogel: 修改popuplist的滚动条问题
- wPgg1es  : edit控件增加背景色和显示滚动条显示功能
- starrier : 优化文件拖拽等代码
- Samuel : 修改树控件的一些问题

还有很多提出改进建议和改进方法的人，没有一一列出来，欢迎大家参与这个开源项目，共同完善。
