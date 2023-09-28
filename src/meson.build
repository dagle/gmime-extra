source_c = [
  'gmime-extra.c',
  'gmime-extra-filter-reply.c',
  'gmime-extra-notify.c',
  'gmime-extra-address-compare.c',
  ]

source_h = [
  'gmime-extra.h',
  'gmime-extra-filter-reply.h',
  'gmime-extra-notify.h',
  'gmime-extra-address-compare.h',
  ]

deps = [
  dependency('gmime-3.0'),
  dependency('glib-2.0'),
  dependency('gobject-2.0'),
  dependency('gio-2.0'),
]
galore_lib = library('gmime-extra', source_c, dependencies : deps, install: true)
gnome = import('gnome')

if true
  gir_args = [
    '--quiet',
    '--warn-all',
    ]

  filter_glib_gir = gnome.generate_gir(
    galore_lib,
    sources: source_c + source_h,
    namespace: 'GMimeExtra',
    nsversion: '0.1',
    symbol_prefix: ['g_mime_extra', 'gmime_extra'],
    includes: [ 'GObject-2.0', 'Gio-2.0', 'GMime-3.0'],
    dependencies: deps,
    extra_args: gir_args,
    fatal_warnings: true,
    install: true,
    )
else
  # Just warn and quit
  json_glib_gir = []
endif
