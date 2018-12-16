### VitaSocketController
PSVitaのボタン/スティック入力をUDPで指定したIPアドレス/ポートに送ります。

### 使い方
main.c内のIPとPORTを送信先のIPアドレスとポートに変えてください。
その後  

```
cmake .
make
```

でコンパイルします。
出来た.vpkをVitaShell等でインストールしてください。

### 参考
[socket_ping](https://github.com/vitasdk/samples/tree/master/socket_ping)  
[vita_netdbg](https://gist.github.com/xerpi/e426284df19c217a8128)
