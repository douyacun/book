* mongodb curd


# 插入

插入的时候如果集合不存在，会创建集合

## _id 字段 
存储于集合的每一个文档都需要一个唯一的 `_id` 字段作为 *primary_key* ， 如果遗漏会自动生成，


## db.collection.insertOne() 向集合中插入单个文档
- insert 支持插入多个文档
- model 返回true，不管多少条
> model 指的是laravel中的model， 以下均是
```
db.collection.insertOne(
    {
        _id: 1,
        name: "douyacun",
        age: 25,
        status: "p"
    }
)
```

## db.collection.insertMany() 向集合中插入多个文档
```
db.collection.insertMany([
     {_id: 2, name: "bob", age: 42, status: "A", },
     {_id: 3, name: "ahn", age: 22, status: "A", },
     {_id: 4, name: "xi", age: 34, status: "D", }
]);
```

# 查询

## db.collection.find(<query filter>, <projection>)
- 可以直接在find中指定查询条件
- model 中的find， 只能用_id作为查询条件，返回单个模型

## =
```
db.user.find({_id: 5});
```

## and
- $lt: 30 小于 30 的
```
db.user.find({_id: 5, age: {$lt: 30}});
```

## or
```
db.user.find(
    {
        $or: [
            {age: {$gt: 30}},
            {age: {$lt: 20}}
        ]
    }
);
```

## and & or
```
db.user.find(
    {
        $or: [
            {age: {$gt: 30}},
            {age: {$lt: 20}},
        ],
        status: "v"
    }
)
```