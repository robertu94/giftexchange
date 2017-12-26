#Gift Exchange Program

This program is designed to make gift exchange lists for family gift exchanges.

It expects two input files: families.txt and restrictions.txt

families.txt is expected to contain a list of family names followed by family member first names.
Families will not be matched with people in their own family.
Both the family names and family member names are expected to be unique.
It supports comments if a line begins with a `#`.

```txt
myfamily mother father brother me
relatives aunt uncle cousin
grandparents grandma grandpa
```

restrictions.txt is expected to contain a list of givers and receivers who may not be matched.
For example, a person may not get a gift for the person the got last year or grandma can't get for the grandchildren.
Note that `giver reciever` is not the same as `reciever giver`.
If it is desired to have both restrictions, list both.
It supports comments if a line begins with a `#`.

```txt
#grandma and grandpa can't get for grandchildren
grandma brother 
grandma me 
grandma cousin
grandpa brother 
grandpa me 
grandpa cousin
```

The program is run like so:

```bash
if [[ ! -d 'build' ]]; then
	meson build
	ninja -C build
fi
./build/gifts ./families.txt ./restrictions.txt
```
