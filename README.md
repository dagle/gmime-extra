# GMime-Extra 
GMime extra is a library for bringing extra functionality to GMime

## What is GMime-Extra? 
GMime-Extra adds extra functionality to the GMime library. The reason for lib
is to try on new functionality to GMime and to add extra functionality not fit
for GMime. GMime is a parser library for parsing and building emails.
GMime-Extra will contain a lot of functionality that is beyond that scope,
trying to fill in a gap between what just parsing and functionality most MUA's
(or scripts of that nature) would need to implement. Removing the need for for
people to reinvent the wheel.

## Why not just upstream?
Multiple reason. Interfaces here are quite unstable and can change at any time
where GMime is stable. Also, a lot of the functionality doesn't belong in a parser.

# What does GMime-Extra contain?
- gmime-extra-filter-reply, an fast an easy filter than turns a part into a
reply.
- gmime-extra-address-compare, functions for comparing addresses, but also
other addresses related functions. Like finding out which one of your emails
were used to address you (so you can reply from the correct one).
- gmime-extra-sq, another crypto engine for pgp, which adds functionality like
autocrypt (with extra helpers) and pep. [sequoia-pgp](https://sequoia-pgp.org/)
- gmime-extra-notify, both DeliveryStatus and DispositionNotification

## License Information

The GMime library is Copyright (C) 2023 Per Odlund and is licensed under the LGPL v2.1

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

## Requirements

GMime-Extra uses meson to build, this is because it needs both rust and C to compile.

- GMime-Extra is currently tested to work with GMime 3.2.14 or later, others
might work but are untested. 
