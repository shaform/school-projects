News Meter
==========

News Meter is a news portal that provides news search and automatic sentiment analysis for different news sources.
It can retrieve the news from Google News or use a CIRB format database.

Note that this version does not include the sentiment analysis engine, you need to manually put a score-list file inside the wm directory.
Each line of the score-list file must contian a floating number representing the polarity for each line in file-list.

媒體立場儀是一個提供新聞搜尋與媒體立場分析的新聞入口網站。
她可以搭配 Google News 作為新聞來源，也可以用 CIRB 格式的資料作為新聞資料庫。

注意到這個版本並沒有提供新聞立場分析的程式碼，你需要自行在 wm 資料夾建立一個 score-list 檔案。這個檔案每一行都是一個浮點數，代表在 file-list 裡每則新聞的正負傾向。

![homepage](https://raw.githubusercontent.com/shaform/newsmeter/master/static/img/screenshots/home.png)

![news sentiment](https://raw.githubusercontent.com/shaform/newsmeter/master/static/img/screenshots/sentiment.png)

## Author

Yong-Siang Shih

News Meter is originally a programming project for Web Retrieval and Mining course Spring 2014 at National Taiwan University.
The original authors include:

* Junjie Wang - Responsible for news crawling.
* 劉宇錚 - Responsible for sentiment analysis.
* Yen-Chi Shao - Responsible for sentiment analysis.
* Yong-Siang Shih - Responsible for everything in this git repository.

## License

Python code is licensed under GPLv3, see [LICENSE](https://raw.githubusercontent.com/shaform/newsmeter/master/LICENSE) for detail.

The webpage design is licensed under [Attribution-NonCommercial-ShareAlike 2.0 Generic](https://creativecommons.org/licenses/by-nc-sa/2.0/).

[Magnific Popup](http://dimsemenov.com/plugins/magnific-popup/) is licensed under [MIT License](https://raw.githubusercontent.com/dimsemenov/Magnific-Popup/master/LICENSE).
