# OpenGL Project
## 开发日志
### 2018/11/20
#### 杨建伟
一、创建了github仓库
二、Merge策略如下：
	开发初期各自造各自的轮子，造好后放到自己的文件夹里push(经测试可用)
	每次提交写好这次提交的内容概要
	提交后，写类似这样的开发日志。
	为了能知晓彼此的工作进展，建议造了一个小轮子就commit一次
### 2018/11/21
#### 杨建伟
基本逻辑框架完成，由于太晚要睡觉了，只打了关键注释（体育早课神烦)

目录下多了一堆文件，Object对象我打算用于做基类，完成顶点储存和draw功能，但需要一个导入模型的功能，留给后人

然后剩下的几个就是当时我们讨论的了，啊对，为了方便看一点，我还没有取消y方向上的位移（也就是说可以飞天现在，但不推荐用炸弹的时候飞天，可能出BUG？

将道具使用加在键盘回调函数里了，具体在main函数中，R是放脚, X用炸弹，P用能量包，以上按键对应道具首字母，啊对，因为XM值没法显示，所以用能量包没法测试

目前炸弹是不分敌我的，伤害为100（方便看效果，炸到就消失2333）

H是hack， hack出道具的概率是50%，目前也不分敌我Portal的， 每10s可以hack一次,成功hack Portal的颜色会变

啊对，注意使用距离和角度。
### 2018/11/22
#### 杨建伟
1. 把昨天那个炸脚的BUG修复了

2. 进一步抽象代码，完成了Object基类，现在类更精简了！

3. 完成了Enemy的框架，打算回头有时间再写，然后明天分一下阵营逻辑就暂时告一段落


