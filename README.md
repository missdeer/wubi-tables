[Fcitx5-Android](https://github.com/fcitx5-android/fcitx5-android)使用的码表

目前包括3个码表：
1. 极点五笔86
2. QQ五笔86
3. 樊86五笔，基于QQ五笔86的词频顺序合并了极点五笔86的词库

导入方法见：https://fcitx5-android.github.io/faq/

码表源文件在.txt文件中，可使用任意纯文本编辑器打开编辑修改，之后在Linux命令行使用如下命令生成二进制码表文件：

```bash
libime_tabledict jidian86wubi.txt jidian86wubi.main.dict
```

或

```bash
libime_tabledict qq86wubi.txt qq86wubi.main.dict
```
