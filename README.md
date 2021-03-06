# EzAvg
一个简单的文字冒险游戏引擎

## 文件目录
src/
源码

scene/
场景脚本

lua
lua脚本

lua/behaviour
精灵行为脚本，处理动画和交互相关的内容

lua/task
任务，与场景脚本相配合

## 引擎简介
这是一个超超超超超简陋的AVG文字冒险游戏引擎，EzAvg制作的游戏由场景脚本所构成，并通过Lua脚本来创建和控制精灵。

### 框架
引擎启动后，首先会创建 *eaApplication* 类，该类负责管理Lua和SDL窗体与事件。在游戏启动后，首先会加载Lua脚本 *Game.lua* ，这个脚本中应有对应用程序的信息相关的设置。为了方便更改，创建了 *Config.lua* 来储存基本应用信息，内置的 *Game.lua* 会自动把其中的信息设置到应用程序当中

在 *Game.lua* 中，与初始化相关代码的最后是加载初始场景脚本 *scene/Startup.scene* ，所有场景脚本均应放入 *scene* 文件夹当中

每个场景均与一个场景脚本相互绑定，一旦场景脚本运行结束，就只剩下精灵行为还在运行。尽量避免这种情况，除非对场景机制十分了解。因为这样将会失去对场景的控制，一旦所有行为运行结束，场景将会静止不动

场景脚本由块组成，而块中，只有 *任务块* *文本块* *Lua块* 是真正会运行的。在运行到 *任务块* 的时候，会在 *task* 文件夹中自动寻找对应的lua脚本，并且知道任务结束之前，不会往下运行脚本。在运行到 *文本块* 的时候，会调用注册到该场景的 *文本块处理器* 。在运行到lua块的时候，会执行lua脚本

精灵，是场景中可见的元素，目前精灵包括 *图像精灵* 和 *文本精灵* ，精灵和行为脚本相互配合，可以实现很多效果

行为脚本与任务相似，但是不同点是其不会阻塞场景脚本的运行，也就是说可以同步执行，这一特性使得其更加适合去制作动画与交互

#### 域
域是引擎内部实现中用到的一个概念，域可以继承自其父域，在子域内创建对象不会影响到父域。域又分成两组类型，虚域和实域，在代码中并未明显区分二者，在这里做出区分主要是为了让框架更清晰。实域是与Lua脚本向对应的，一个实域中需包含外部lua代码，而虚域是由引擎创建的，并不直接真实lua代码。在引擎中有如下域：
1. eaApplicaiton
	父域为全局域，引擎中所有域均直接或间接继承于此
1. eaScene
	场景域，继承自eaApplication，包含对象 *scene* ，即当前场景对象。可通过 *scene* 来找精灵
	例如我们创建了一个名字为"FirstSprite"的精灵，那么我们就可以在Lua块，或者内嵌Lua中使用 `scene.FirstSprite` 来获取对应的精灵
1. eaSprite
	精灵域，继承自拥有该精灵的 *exScene* ，包含对象 *sprite* 因为继承关系，其中也包含 *scene* 。可以通过 *sprite* 对象来获取和修改精灵属性，如对于文本精灵，其包含属性 *text* ，我们可以通过 `textSprite.text="HelloWorld"` 来更改其文本
1. eaTask
	任务域，继承自创建该任务的 *eaScene* 。实域，与 *任务脚本* 相对应
1. eaBehaviour
	行为域，继承自拥有该脚本的 *eaSprite* 。实域，与 *行为脚本* 相对应

#### 布局
引擎中，所有精灵对象均存在于 *精灵盒* 中，精灵盒的大小由拥有该精灵的 *精灵组* 管理（虽然也提供了从其他地方修改精灵和的方法，但是不建议使用），精灵盒内的精灵对象的具体位置由margin来确定，margin为精灵距离精灵盒边界的距离，当margin的属性中有为0项的时候，这项内容会由精灵的size自动计算，当up和bottom、left和right同时为0时，计算bottom、right

引擎中内置了几种布局脚本，所有布局均行为脚本均需创建在 *精灵组* 上

#### 精灵
精灵是场景中的基础元素，一切均为精灵。所有精灵均包含以下属性：

|  属性 | 值类型 |  说明 |
| ------------ | ------------ | ------------ |
| name  |  string | 精灵名称 |
| type  |  string | 精灵类型 |
| enabled | boolean  |  精灵是否启用，若未启用则不会刷新 |
| destroyed | boolean  |  精灵是否销毁，若已销毁则会在下一帧中移除 |
| zOrder | integer  |  精灵在Z轴上的顺序，不同精灵组之间的精灵的zOrder是相互独立的 |
| margin | table(<br>number,<br>number,<br>number,<br>number)<br>table(<br>number,<br>number)  |  精灵距离精灵盒边界的距离，从小到大依次是up,right,button,left，或者直接给up和left |
| size | table(<br>number,<br>number)  |  精灵的大小，从小到大依次是width, height |
| box | table(<br>number,<br>number,<br>number,<br>number)  |  精灵盒的大小，从小到大依次是x,y,width,height |
| rect | table(<br>number,<br>number,<br>number,<br>number)  |  精灵渲染区域大小，从小到大依次是x,y,width,height |
| alpha | number | 精灵透明度，最终影响精灵透明度的还有父精灵组的透明度 |

|  方法 | 返回值 |  说明 |
| ------------ | ------------ | ------------ |
| addBehaviour(string name, string type) | sprite | 为精灵创建类型为type的名为name的行为脚本 |

引擎中内置了几种简单精灵：
- image 精灵
	负责显示图像，场景背景、按钮背景等均由此实现

|  属性 | 值类型 |  说明 |
| ------------ | ------------ | ------------ |
| image  | string | 精灵图像名，相对于image文件夹的路径 |
| color  | table(<br>number,<br>number,<br>number,<br>number) | 精灵颜色，从小到大依次是R,G,B,A |

- text 精灵
	负责显示文本

|  属性 | 值类型 |  说明 |
| ------------ | ------------ | ------------ |
| text  | string | 显示的文本 |
| fontName  | string | 字体名，相对于font文件夹的路径 |
| fontSize  | number | 字体大小 |
| horizontalLayout  | string | 文本水平布局，可能的值：`left` `center` `right` |
| verticalLayout  | string | 文本垂直布局，可能的值：`up` `center` `down` |
| color  | table(<br>number,<br>number,<br>number,<br>number) | 文字颜色，从小到大依次是R,G,B,A |
| shadowColor  | table(<br>number,<br>number,<br>number,<br>number) | 文字阴影颜色，从小到大依次是R,G,B,A |
- group 精灵
	是精灵的容器，一切精灵均由精灵组创建，一切精灵均由精灵组管理

|  属性 | 值类型 |  说明 |
| ------------ | ------------ | ------------ |
| autoLayout  | boolean | 是否启用内置自动布局，即自动将子精灵填充，与布局行为脚本配合 |
| propertyTable  | table(<br>string,<br>string) | 属性表，将内部精灵的属性暴露到外部 |

|  方法 | 返回值 |  说明 |
| ------------ | ------------ | ------------ |
| addSprite(string name, string type) | sprite | 创建类型为type的名为name的精灵 |
| iter() | ---- | 迭代器，在for循环里遍历精灵组内包含的sprite对象 |

属性表可简化对精灵组内部精灵属性的修改，对于一个按钮来说，我们需要将text属性绑定到内部精灵Text的text属性上，此时我们只需要添加{text="Text.text"}到属性表里即可

引擎会对应每个精灵创建一个lua对象：sprite，其存在于该精灵所创建的域当中。对于精灵组，可以使用 sprite.childName 来访问其拥有的子精灵

#### 场景
场景可以理解成是一个精灵组，其中默认绑定了很多与场景相关的属性。当调用GameInit后，会把GameScene样式应用到场景上，GameScene样式内的属性如下：
|  属性 | 值类型 |  说明 |
| ------------ | ------------ | ------------ |
| text | string | 主对话框，负责显示文本块内的内容 |
| printedAllText | boolean | 只读，是否显示完所有文本 |
| arg | any | 场景创建时的参数，可以为nil |
| popSceneResult | any | 弹出场景的返回值，只读 |

|  方法 | 返回值 |  说明 |
| ------------ | ------------ | ------------ |
| popScene(string name, any arg) | nil | 弹出场景，场景弹出后暂停下层场景的刷新 |
| close(any result) | nil | 关闭当前场景并设置返回值 |

#### 扩展
若希望通过加入自己的Lua脚本来扩展引擎，请必须详细查看本节
游戏内支持保存与加载，但是需要遵从一定规则：
- 所有任务和行为内需要保存的数据必须设置到返回的表内，保存时会自动保存表内的数字、布尔、表和字符串
- 不要过于依赖start方法来进行初始化，要记住，如果保存时已经调用过start，加载后就不会再调用
- 任务和精灵行为脚本很相似，但是二者用处不同。如果任务脚本一直运行会造成场景的阻塞，会停在这个任务不往下执行，但是精灵行为脚本一直执行也不会造成阻塞。可以利用场景脚本的阻塞与精灵行为结合来实现按钮与分支选择的功能
- 不要使用协程，不要使用协程，不要使用协程
- 调试阶段建议在Lua中加入自己的异常处理
- 无需担心精灵和场景的属性，其所有属性均已自动保存

### 场景脚本
场景脚本是EzAvg游戏的基础，而 *块* 是场景脚本的基础，一共有五种 *块* ：
1. 注释块
以#开头，其后的一整行为注释块
1. 任务块
以:开头的一行或多行为任务块，任务块形式如下：
:[TaskName] arg1=value arg2=value ......
其中value支持四种类型：
	1. 数字
	整数和小数均可使用
	1. 字符串
	左右两端为引号的一串无换行文本，支持内嵌Lua字符串对象，例如 `"你好{return '世界'}"` 将会在运行时变成 `"你好世界"`
	1. 枚举
	与字符串类似，但是只允许由数字、字幕构成，两端不需要引号。所有枚举类型均会以字符串读入
	1. 数组
	由方括号包围，如 `[a,"b",{return "c"},4]`
	1. Lua对象
	通过在运行时调用Lua脚本动态获取结果，由花括号包围，并包含一个返回值，例如 `:Wait time={return 123}` 与 `:Wait time=123` 等价。对于单行Lua对象，可以去掉return，是一种简化的写法
	
	所有任务都会交给对应的Lua任务脚本执行
1. Lua块
	由花括号包围的一串Lua代码
1. 文本块
	对SetText任务的简化，由任意非其他块首字符开头，并由空行结尾。程序会执行任务 `:SetText text=str` 其中str为文本块的字符串。文本块字符串也支持内嵌Lua字符串对象。
	
下边是一段场景脚本的示例
```
# 井号开始的一行为注释
# 任务块示例
:ExampleTask1
:ExampleTask2 val1 = 123.456
:ExampleTask3 val1=enum val2="str" val3=[a,r,r,a,y] val4=[{return "lua"},{return "array"}] val5={'c'}
# Lua块示例
{
    doLuaFunction()
}
# 文本块
文本块支持多行
最后一行需要为空行
否则文本块不会结束
    
# 上一行为空行，文本块结束
```
### Lua任务脚本
一个完整的Lua任务脚本如下：
```lua
local p = {}

-- 储存当前任务状态
p.enabled = true

-- 任务刷新
function p.update()
end

-- 任务启动，其中 args 的类型为 table ，对应着在场景脚本中任务块参数
function p.start(args)
end

-- 返回任务脚本对象
return p
```
当任务创建后，首先会调用其start方法，并且在下一帧开始调用其update方法。如果 `enabled=false` 或者 `enabled=nil` ，将不会调用update方法，也就是说，如果不需要刷新的任务可以简化为：
```lua
local p = {}

-- 任务启动，其中 args 的类型为 table ，对应着在场景脚本中任务块参数
function p.start(args)
end

-- 返回任务脚本对象
return p
```
#### 内置任务脚本
##### Init
初始化场景

| 必须 |  名称  |  类型  |     说明    |
| :---: | :----: | :-----: | :--------: |
|       | arg    | valuePair | 初始化属性 |
|| ... | ... | ... |

##### SpriteCreate
创建精灵

| 必须 |  名称  |  类型  |     说明    |
| :---: | :-----: | :-----: | :--------: |
|   *   | sprite | string | 精灵名称 |
|       | style  | string | 精灵样式 |
|       | arg..  | valuePair.. | 需要设置的精灵参数 |

##### SpriteRemove
移除精灵

| 必须 |  名称  |  类型  |     说明    |
| :---: | :-----: | :-----: | :--------: |
|   *   | sprite | string | 精灵名称 |

##### SpriteSet
设置精灵属性

| 必须 |  名称  |  类型  |     说明    |
| :---: | :----: | :-----: | :--------: |
|       | sprite | string | 精灵名称，为空则为scene |
|       | arg    | valuePair | 需要设置的精灵参数 |
|| ... | ... | ... |

##### BehaviourAdd
向精灵绑定行为
| 必须 |  名称  |  类型  |     说明    |
| :---: | :----: | :-----: | :--------: |
|       | sprite | string | 精灵名称，若未指定则默认为scene |
|   *   | behaviour  | string | 行为名称 |
|   *   | type    | string | 行为类型 |

##### BehaviourRemove
移除精灵行为
| 必须 |  名称  |  类型  |     说明    |
| :---: | :----: | :-----: | :--------: |
|       | sprite | string | 精灵名称，若未指定则默认为scene |
|   *   | behaviour  | string | 行为名称 |

##### Goto
场景内跳转

| 必须 |  名称  |        类型        |     说明    |
| :---: | :-----: | :--------------: | :--------: |
|   *   |  pos   | string\number | 跳转目标，当pos为$时，代表跳转到当前块 |

##### Wait
等待

| 必须 |  名称  |   类型   |     说明    |
| :---: | :-----: | :-------: | :--------: |
|   *   | name | number | 等待时间 |

| 必须 |  名称  |   类型   |     说明    |
| :---: | :-----: | :-------: | :--------: |
|       | sprite | string | 精灵名称，若未指定则默认为scene |
|   *   | behaviour | string | 行为名称 |
|       | keep | bool | 是否结束后保留行为 |

##### GameInit
初始化游戏，创建对话框等

##### GameDialogHide
隐藏游戏对话框

##### GameDialogShow
显示游戏对话框

### Lua精灵行为脚本
一个完整的Lua任务脚本如下：
```lua
local p = {}

-- 储存当前行为状态
p.enabled = true

-- 行为刷新
function p.update()
end

-- 行为启动
function p.start()
end

-- 返回行为脚本对象
return p
```
Lua精灵行为脚本与任务脚本很相似，区别是start方法不再需要参数。所有参数均被设置到为行为脚本创建的表中，之所以设计这一区别，是因为任务脚本的参数可变性要比行为脚本大的多，行为脚本的参数基本固定，而任务脚本的参数多变，如内置的 *SetSprite* 脚本中，根本无法预知有什么参数，这样把参数绑定到对应脚本的表中的意义就不大了。
### 消息机制
游戏中采用消息机制，可以向精灵发送消息，然后消息会把收到的消息转发给挂载到精灵的所有行为上，引擎中包含如下几种消息：
- onMouseDown 当鼠标按下精灵时发送到精灵
- onMouseUp 当鼠标在精灵上抬起时发送到精灵
- onMouseEnter 当鼠标进入精灵时发送到精灵
- onMouseLeave 当鼠标离开精灵时发送到精灵