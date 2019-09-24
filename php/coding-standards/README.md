# php 代码规范

- 推荐为项目单独配置
- [phpcs助手网站](http://edorian.github.io/php-coding-standard-generator/#phpcs)

## phpcs 设置

### 安装
composer.json
```json
{
    "require-dev": {
        "squizlabs/php_codesniffer": "3.*"
    }
}
```

phpmd.xml
```xml
<?xml version="1.0" encoding="UTF-8"?>
<ruleset name="sspapi">
    <description>Created with the PHP Coding Standard Generator.
        http://edorian.github.com/php-coding-standard-generator/
    </description>
    <!--不能声明空的结构体-->
    <rule ref="Generic.CodeAnalysis.EmptyStatement"/>
    <!--简但for循环 写为 while
    class Foo {
      public function bar($x) {
        for (;$x > 5;) { /* ... */ } // No Init or Update part, may as well be: while ($x > 5)
      }
    }
    -->
    <rule ref="Generic.CodeAnalysis.ForLoopShouldBeWhileLoop"/>
    <!-- 报告for循环可能混乱的情况
     function bar($x) {
       for ($i = 0; $i < 10; $i++) {
         for ($k = 0; $k < 20; $i++) {
           echo 'Hello';
         }
       }
     }
    -->
    <rule ref="Generic.CodeAnalysis.JumbledIncrementer"/>
    <!-- if else 可能是无条件
    if(true){}
    -->
    <rule ref="Generic.CodeAnalysis.UnconditionalIfStatement"/>
    <!--不允许未使用的函数参数-->
    <rule ref="Generic.CodeAnalysis.UnusedFunctionParameter"/>
    <!--不允许使用简单地调用其父级的不必要的重写方法。
    class Foo {
      public function __construct($foo, $bar) {
        parent::__construct($foo, $bar);
      }
    }
    -->
    <!--可能没有任何TODO评论
    function worldPeace() {
      // TODO: Achieve this
    }
    -->
    <rule ref="Generic.Commenting.Todo"/>
    <!-- 内联控制结构，如
    if($x)continue;//不允许
    while(true) $x++; if($x > 10); break; // 不允许
    if($x)
        continue;// 不允许
    -->
    <rule ref="Generic.ControlStructures.InlineControlStructure"/>
    <!--控制每一行的长度，最长150个字符-->
    <rule ref="Generic.Files.LineLength">
        <properties>
            <property name="absoluteLineLimit" value="150"/>
        </properties>
    </rule>
    <!-- 等号对齐
    //有效
    $stuff      = 1;
    $otherStuff = 2;
    //无效
    $stuff = 1;
    $otherStuff = 2;
    //有效
    $myVar           = 1
    $myOtherVariable = 2;

    $x = 100; // 中间隔了一行，不需要对齐
    while($x--) {
      echo $x, PHP_EOL;
    }
    -->
    <rule ref="Generic.Formatting.MultipleStatementAlignment"/>
    <!--
    //Valid
    $x = (string) 1;
    //Invalid
    $x = (string)1;
    //Also invalid
    $x = (string)  1; // Only 1 space allowed!
    -->
    <rule ref="Generic.Formatting.SpaceAfterCast"/>
    <!-- 函数声明 { 必须与function 同行
    //Valid
    function foo() {
        // ...
    }
    //Invalid
    function foo()
    {
        // ...
    }
    -->
    <rule ref="Generic.Functions.OpeningFunctionBraceKernighanRitchie"/>
    <!--函数复杂度提示 10提示 20报错-->
    <rule ref="Generic.Metrics.CyclomaticComplexity">
        <properties>
            <property name="complexity" value="10"/>
            <property name="absoluteComplexity" value="20"/>
        </properties>
    </rule>
    <!-- if else 嵌套这么多不晕吗？最多嵌套8个
    if($a) {
      if($b) {
        if($c) {
          if($d) {
            if($e) {
              if($f) {
                echo 'Stuff';
              }
            }
          }
        }
      }
    }
    -->
    <rule ref="Generic.Metrics.NestingLevel">
        <properties>
            <property name="nestingLevel" value="5"/>
            <property name="absoluteNestingLevel" value="8"/>
        </properties>
    </rule>
    <!--不允许使用php4的构造函数-->
    <rule ref="Generic.NamingConventions.ConstructorName"/>
    <!--常量必须是大写的-->
    <rule ref="Generic.NamingConventions.UpperCaseConstantName"/>
    <!--函数命名使用单驼峰-->
    <rule ref="Generic.NamingConventions.CamelCapsFunctionName"/>
    <!--不允许使用废弃的函数-->
    <rule ref="Generic.PHP.DeprecatedFunctions"/>
    <!--true/false/null 必须是小写-->
    <rule ref="Generic.PHP.LowerCaseConstant"/>
    <!--
    // 不允许
    echo "hi" . "ho";
    echo "hi"
        ."there"
        ."how"
        ."are"
        ."you?";
    -->
    <rule ref="Generic.Strings.UnnecessaryStringConcat"/>
    <!--class {必须另起一行-->
    <rule ref="PEAR.Classes.ClassDeclaration"/>
    <!--不允许使用#注释，phpstorm不支持快捷键-->
    <rule ref="PEAR.Commenting.InlineComment"/>
    <!--
    if (($condition1
        || $condition2)
        && $condition3
        && $condition4
        && $condition5
    ) {
    }
    -->
    <rule ref="PEAR.ControlStructures.MultiLineCondition"/>
    <!--函数 有默认值的参数 往后放-->
    <rule ref="PEAR.Functions.ValidDefaultValue"/>
    <!--
    $object
        ->setBar($foo)
        ->setFoo($bar);
    -->
    <rule ref="PEAR.WhiteSpace.ObjectOperatorIndent"/>
    <!--{}必须对齐-->
    <rule ref="PEAR.WhiteSpace.ScopeClosingBrace"/>
    <!--ps1 每个类必须独立于一个文件中，并且必须位于至少一个级别深度的命名空间中-->
    <rule ref="PSR1.Classes.ClassDeclaration"/>
    <!--文件结尾必须有空行-->
    <rule ref="PSR2.Files.EndFileNewline"/>
    <!--命名空间声明后必须有一个空行-->
    <rule ref="PSR2.Namespaces.NamespaceDeclaration"/>
    <!--确保未将比较值分配给变量
    // 无效
    $a == $b
    -->
    <rule ref="Squiz.PHP.DisallowComparisonAssignment"/>
    <!--
    // Forbidden
    for($i = 0; $i < count($i); ++$i) {
    }
    // Allowed
    $amountOfElements = count($i);
    for($i = 0; $i < $amountOfElements; ++$i) {
    }
    -->
    <rule ref="Squiz.PHP.DisallowSizeFunctionsInLoops"/>
    <!--警告使用eval函数-->
    <rule ref="Squiz.PHP.Eval"/>
    <!--禁止在函数内部定义函数-->
    <rule ref="Squiz.PHP.InnerFunctions"/>
    <!--确保对内置PHP函数的所有调用都是小写的-->
    <rule ref="Squiz.PHP.LowercasePHPFunctions"/>
    <!--警告可能永远不会执行的代码-->
    <rule ref="Squiz.PHP.NonExecutableCode"/>
    <!--确保 成员属性 使用 修饰符 public/private/protected-->
    <rule ref="Squiz.Scope.MemberVarScope"/>
    <!--确保 成员方法 使用 修饰符 public/private/protected-->
    <rule ref="Squiz.Scope.MethodScope"/>
    <!--确保静态类不用this-->
    <rule ref="Squiz.Scope.StaticThisUsage"/>
    <!--确保强制转换语句不包含空格
    // 无效
     $a = (string ) $b;
    -->
    <rule ref="Squiz.WhiteSpace.CastSpacing"/>
    <!--
    // Forbidden
    echo"hi";
    echo   "hi";
    $x = new   StdClass();
    // Allowed
    echo "hi";
    $x = new StdClass();
    -->
    <rule ref="Squiz.WhiteSpace.LanguageConstructSpacing"/>
    <!--逻辑运算符之前和之后都有一个空格    $a && $b-->
    <rule ref="Squiz.WhiteSpace.LogicalOperatorSpacing"/>
    <!--成员属性声明之间有一个空行-->
    <rule ref="Squiz.WhiteSpace.MemberVarSpacing"/>
    <!--对象方法调用 $object->func() 之间没有空格-->
    <rule ref="Squiz.WhiteSpace.ObjectOperatorSpacing"/>
    <!--确保闭括号位于新行并正确缩进。-->
    <rule ref="Squiz.WhiteSpace.ScopeClosingBrace"/>
</ruleset>
```

使用 `git hooks` 和 `composer scripts` 设置提交时校验代码

composer.json
```json
"scripts": {
    "post-install-cmd": [
        "bash scripts/setup-hooks.sh"
    ]
}
```

setup-hooks.sh
```shell
#!/bin/bash;

PROJECT=`php -r "echo dirname(dirname(realpath('$0')));"`
cp ${PROJECT}/scripts/pre-commit.sh ${GIT_DIR}/hooks/pre-commit
chmod +x ${PROJECT}/.git/hooks/pre-commit
```

pre-commmit
```shell
#!/bin/sh

PROJECT=`php -r "echo dirname(dirname(dirname(realpath('$0'))));"`
PHPCS=$PROJECT/vendor/bin/phpcs
PHPMD=$PROJECT/vendor/bin/phpmd
STAGED_FILES_CMD=`git diff --cached --name-only --diff-filter=ACMR HEAD | grep \\\\.php`

# Determine if a file list is passed
if [ "$#" -eq 1 ]
then
	oIFS=$IFS
	IFS='
	'
	SFILES="$1"
	IFS=$oIFS
fi
SFILES=${SFILES:-$STAGED_FILES_CMD}

for FILE in $SFILES
do
    # php valite
    php -l -d display_errors=0 $PROJECT/$FILE
    # phpcs
    if [ $? != 0 ]
    then
        echo "Fix the error before commit."
        exit 1
    fi
    # phpmd validate 这里时phpmd校验下面会说
    ${PHPMD} ${FILE} text ${PROJECT}/phpmd.xml
    if [ $? != 0 ]
    then
        echo "Fix that PHPMD violation error before commit."
        exit 1
    fi
    FILES="$FILES $PROJECT/$FILE"
done

if [ "$FILES" != "" ]
then
    echo "Running Code Sniffer. Code standard PSR2."
    $PHPCS --standard=PSR2 --colors --encoding=utf-8 -n -p ${FILES}
    if [ $? != 0 ]
    then
        echo "Fix that PHPCS violation error before commit"
    fi
fi

exit $?
```
> 参考： https://github.com/squizlabs/PHP_CodeSniffer

# PHPMD 规则验证

phpmd.xml
```xml
<?xml version="1.0"?>
<ruleset name="00h.tv" >
    <rule ref="rulesets/codesize.xml/TooManyFields" />
    <!--- 一个类中不能有太多fields字段,默认值15 -->
    <rule ref="rulesets/codesize.xml/NPathComplexity" />
    <!--- 一个方法中NPath复杂性,默认值200 -->
    <rule ref="rulesets/codesize.xml/ExcessiveMethodLength" />
    <!--- 相同的方法执行同一功能,默认值100 -->
    <rule ref="rulesets/codesize.xml/ExcessiveClassLength" />
    <!--- 一个类的最大行数,默认值1000 -->
    <rule ref="rulesets/codesize.xml/TooManyMethods" />
    <!--- 一个类的最大方法数,默认值25 -->
    <rule ref="rulesets/controversial.xml/CamelCaseClassName" />
    <!--- 以驼峰式命名类 -->
    <rule ref="rulesets/controversial.xml/CamelCaseParameterName" />
    <!--- 以驼峰式命名参数名字 -->
    <rule ref="rulesets/controversial.xml/CamelCaseVariableName" />
    <!--- 以驼峰式命名变量名字 -->
    <rule ref="rulesets/design.xml/DevelopmentCodeFragment" />
    <!--  代码中不要有var_dump(), print_r()等这种调试输出结构 -->
    <rule ref="rulesets/design.xml/EvalExpression" />
    <!--  代码中不要有eval()这种结构 -->
    <rule ref="rulesets/design.xml/GotoStatement" />
    <!--  代码中不要有goto这种结构 -->
    <rule ref="rulesets/naming.xml/BooleanGetMethodName" />
    <!--  返回值为布尔型而以'getX()'格式命名的方法改写成'isX()' 或者 'hasX()'-->
    <rule ref="rulesets/naming.xml/ConstantNamingConventions" />
    <!--  类/接口常量必须用大写字母定义-->
    <rule ref="rulesets/unusedcode.xml/UnusedLocalVariable" />
    <!--   存在已声明或赋值的局部变量，但是却未使用-->
    <rule ref="rulesets/unusedcode.xml/UnusedPrivateField" />
    <!--  存在已声明或赋值的局部变量，但是却未使用-->
    <rule ref="rulesets/controversial.xml/Superglobals" />
    <!--  全局变量必须封装在对象中，由框架或接口提供 -->
</ruleset>
```
> 参考： https://phpmd.org/rules/index.html

# issue:  PhpStorm 的 Reformat 与 `PHPCS` 推荐样式冲突

```php
public function getExtraBargainTime()
{
    //@formatter:off
    $time = Redis::hget(
        BARGAIN_GROUP_EXTRA_CHANCE_REGISTRY,
        $this->compositeUserAndProduct()
    ) ?? 0;
    //@formatter:on
    return max($time, 0);
}
```

# issue: WSL 环境，PhpStorm VCS 无法执行 git hooks 的 bash 脚本

新建 wslgit.bat，将 PhpStorm git 指定为该文件
```
@echo off
setlocal enabledelayedexpansion
set command=%*
set find=C:\Users\%USERNAME%\AppData\Local\Temp\git-commit-msg-.txt
set replace=/mnt/c/Users/%USERNAME%/AppData/Local/Temp/git-commit-msg-.txt
call set command=%%command:!find!=!replace!%%
If %PROCESSOR_ARCHITECTURE% == x86 (
    C:\Windows\sysnative\bash.exe -c 'git %command%'
) Else (
    bash.exe -c 'git %command%'
)
```
> 这是大牛写的， 我用vagrant

### issue: `phpmd`，`phpcs` 忽略格式验证方法

```php
/**
 * @SuppressWarnings(PHPMD)
 */
``` 
> 参考 https://github.com/squizlabs/PHP_CodeSniffer/wiki/Advanced-Usage#ignoring-parts-of-a-file

# phpstorm 配置

- Editor/Inspections
    - PHP Code Sniffer validation
        - Show warning as Warning
        - show sniff name
    - PHP Mess Detector validation
