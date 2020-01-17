## Process touchpad events in mJS

It seems more smooth and responsive with the touchpad polling done in c.

This example use a single touch pin Touch9. It iuse a dumb method in creating
the events, as I only use one pin. Refer to the mJS documentation, the part
about `s2o` to convert c struct to object in JS, if you use multiple pins and
want to do it more elegantly.
