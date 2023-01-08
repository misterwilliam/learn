# Bash prompt

```
# In .bash_profile
PS1='\[\033[0;32m\][\D{%T}] \[\033[1;34m\]\w\[\033[0m\]\n\$ '
```

This will create a prompt that looks like:
```
[18:41:53] ~
$ 
```

Time will be green. PWD will be blue.

Color escape escape sequences used are:
```
Green:                     \[\033[0;32m\]
Blue:                      \[\033[1;34m\]
Reset to original color:   \[\033[0m\
```

Other special characters used:
```
Time:               \D{%T}
PWD:                \w
$ or # (for root):  \$
```