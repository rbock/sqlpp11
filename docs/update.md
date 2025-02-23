The detailed documentation is still missing here, but here is an example that might help if you have read about [select statements](Select.md).

```C++
db(update(tab).set(tab.gamma = false).where(tab.alpha.in(1)));
```
