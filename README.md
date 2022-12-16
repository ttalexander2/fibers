# fibers

This small library was an experimental attempt to implement a small job system using fibers, 
based on the information described by the GDC talk [Parallelizing the Naughty Dog
engine using fibers](https://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine) by Christian Gyrling,
and the article [Fibers, Oh My!](https://graphitemaster.github.io/fibers/) written by Dale Weiler.

The library is not currently in working condition, however does successfully demonstrate context switching,
as well as the design of a job system using fibers.