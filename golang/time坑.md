---
Title: "go time包踩过的雷"
Keywords: "time,ParseInLocation,Parse,ParseInLocation"
Description: "go AddDate(0, -1, 0)，在7月31号得到的是7月1号"
Author: daning
Label: time
---

## time.AddDate(0, -1, 0)出现的问题

**需求背景：**活动需要在每个月初的时候自动给团队降级

**实现方案：**

- 数据库增加`monthly` 字段标记月份，
  - 比如：在8月1好执行sql `update team set level -= 1, monthly = 8 where monthly = 7`
  - 通过rowsAffected乐观锁判断上一个月是否已经降级
- 定时任务每月1号0点执行，`0 0 0 1 * *`
- 为了防止定时任务不执行或者漏掉，在定时进程重启时埋点出发一次

**定位**

```go
	t := time.Date(2024, 7, 31, 0, 0, 0, 0, time.Local)
	fmt.Printf("7月31号 AddDate(0, -1, 0): %s", t.AddDate(0, -1, 0).Format(time.DateTime))
	
	// 7月31号 AddDate(0, -1, 0): 2024-07-01 00:00:00
```

**改造前代码**

```go
func (c *CopyrightDeificationTeam) Degrade(ctx context.Context, leaderId uint64, level uint32) (bool, error) {
	db := GetDb(ctx)
// 这里贴一下AddDate的官方解释，提到了31号的问题
// AddDate returns the time corresponding to adding the
// given number of years, months, and days to t.
// For example, AddDate(-1, 2, 3) applied to January 1, 2011
// returns March 4, 2010.
//
// AddDate normalizes its result in the same way that Date does,
// so, for example, adding one month to October 31 yields
// December 1, the normalized form for November 31.
	lastMonth := time.Now().AddDate(0, -1, 0)
	res := db.Model(c).Where("leader_id = ? and status != 1 and monthly = ?", leaderId, lastMonth.Month()).UpdateColumns(map[string]interface{}{
		"level":              level,
		"monthly":            time.Now().Month(),
	})
	if res.Error != nil {
		return false, errors.Wrap(res.Error, "CopyrightDeificationTeamRepo.Degrade")
	}
	return res.RowsAffected == 1, nil
}
```

**改造后代码：**

```golang
func (c *CopyrightDeificationTeam) Degrade(ctx context.Context, leaderId uint64, level uint32) (bool, error) {
	db := GetDb(ctx)
	cstSh, _ := time.LoadLocation("Asia/Shanghai")
	_, m, _ := time.Now().In(cstSh).Date()
	if m == 1 {
		m = 13
	}
	res := db.Model(c).Where("leader_id = ? and status != 1 and monthly = ?", leaderId, int(m-1)).UpdateColumns(map[string]interface{}{
		"level":              level,
		"monthly":            m,
	})
	if res.Error != nil {
		return false, errors.Wrap(res.Error, "CopyrightDeificationTeamRepo.Degrade")
	}
	return res.RowsAffected == 1, nil
}
```
