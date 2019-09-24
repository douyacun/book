1、电影正在热映 URL:
 
http://api.douban.com/v2/movie/nowplaying?apikey=0df993c66c0c636e29ecbb5344252a4a
 
2、电影条目信息 URL：
 
https://api.douban.com/v2/movie/subject/:id
 
3、电影条目剧照 URL：
 
http://api.douban.com/v2/movie/subject/:id/photos?apikey=0df993c66c0c636e29ecbb5344252a4a
 
http://api.douban.com/movie/subject/:id/photos?alt=json&apikey=0df993c66c0c636e29ecbb5344252a4a
 
http://api.douban.com/movie/subject/:id/photos?alt=json&apikey=0df993c66c0c636e29ecbb5344252a4a
 
4.影人条目信息 URL：
 
http://api.douban.com/v2/movie/celebrity/:id?apikey=0df993c66c0c636e29ecbb5344252a4a
 
5.影人剧照 URL:
 
http://api.douban.com/v2/movie/celebrity/:id/photos?apikey=0df993c66c0c636e29ecbb5344252a4a
 
6.即将上映 URL：
 
http://api.douban.com/v2/movie/coming?apikey=0df993c66c0c636e29ecbb5344252a4a
 
8.TOP250 URL：
 
http://api.douban.com/v2/movie/top250?apikey=0df993c66c0c636e29ecbb5344252a4a
 
9.电影本周口碑榜 URL：
 
http://api.douban.com/v2/movie/weekly?apikey=0df993c66c0c636e29ecbb5344252a4a
 
10.北美票房榜 URL：
 
http://api.douban.com/v2/movie/us_box?apikey=0df993c66c0c636e29ecbb5344252a4a
 
11.新片榜 URL：
 
http://api.douban.com/v2/movie/new_movies?apikey=0df993c66c0c636e29ecbb5344252a4a
 
附加：
 
热映电影 http://api.douban.com/v2/movie/in_theaters https://api.douban.com/v2/movie/in_theaters?count=8 搜索 https://api.douban.com/v2/movie/search?q=总经理&count=10
 
电影条目搜索
 
http://api.douban.com/v2/movie/search?tag=喜剧
 
即将上映的电影 https://api.douban.com/v2/movie/coming_soon?count=8
 
豆瓣电影Top250 https://api.douban.com/v2/movie/top250?count=8
 
豆瓣小组
 
成员
 
https://api.douban.com/v2/group/husttgeek/members
 
获取小组基本信息：/group/:id? 如：https://api.douban.com/v2/group/husttgeek/
获取话题列表： /group/:id/topics? 如：https://api.douban.com/v2/group/husttgeek/topics
新发话题估计是POST到上面那个地址，没测试
获取某话题评论列表： /group/topic/:id/comments? 如：https://api.douban.com/v2/group/topic/33488193/comments
豆瓣音乐
 
搜索 https://api.douban.com/v2/music/search?q=欧美&count=15?
详情 https://api.douban.com/v2/music/:id
豆瓣图书
 
https://api.douban.com/v2/book/search?q=虚构类&count=8
https://api.douban.com/v2/book/search?q=非虚构类&count=8
https://api.douban.com/v2/book/search?q=旅行&count=8
详情：https://api.douban.com/v2/book/:id
其他
 
某大神的镜像代理66哒http://douban.uieee.com/v2/book/27073559

API Key: 047100f2b7290a250d5302d8257b1e6f
Secret: 133a53d8ad127c9c