
# Dynamic Array

Implementation of the algorithms described in my MSc. Represents a dynamic pattern, subject to character insertion and deletion.

<details>
<summary>Example</summary>

```c++
#include "src/alg/matching.cpp"

dyn_pattern::matching m("aabaacaadaabaaba");
m.insert(0, "aaba");
for (int i = 0; i < m.matches(); i++)
	std::cout << m.match_idx(i) << std::endl;
m.erase(2);
std::cout << m.matches() << std::endl;
```

Output:
```
12
9
0
0
```

Note that the matches returned by `match_idx` might not be sorted.

</details>

Run `make` for a demonstration of the algorithm, using the first $10^7$ digits of $\pi$ as the text. Other texts incude:
	
- `make pi2`: $10^8$ digits of $\pi$;
- `make bible`: the bible;
- `make casmurro`: Dom Casmurro, a famous brazilian book by Machado de Assis.
