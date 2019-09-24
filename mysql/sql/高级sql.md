# 去掉重复的数据:

```
SELECT
    *
FROM
    `scrapy_proxy_ip`
WHERE
    ip IN(
    SELECT
        ip
    FROM
        `scrapy_proxy_ip`
    GROUP BY
        ip
    HAVING
        COUNT(*) > 1
) AND `validated_times` NOT IN(
    SELECT
        MAX(`validated_times`)
    FROM
        `scrapy_proxy_ip`
    GROUP BY
        ip
    HAVING
        COUNT(*) > 1
)
```