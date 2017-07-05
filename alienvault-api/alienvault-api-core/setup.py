#!/usr/bin/env python
# -*- coding: utf-8 -*-
from distutils.core import setup

setup(name="alienvault-api-core",
      version="5.3.0",
      description="The AlienVault API package",
      long_description="""The AlienVault API is an extensible and flexible way to access the platform data and services.
      It is meant to be the kernel of the AlienVault information system and a gate for developers to integrate new applications.
      This package provides the basic methods to access the API""",
      author="AlienVault API team",
      maintainer="AlienVault package developers",
      maintainer_email="debian-devel@alienvault.com",
      url="www.alienvault.com",
      license='LGPLv2',
      classifiers=[
          'Environment :: Web Environment',
          'Environment :: Console',
          'Development Status :: 5 - Production/Stable',
          'Intended Audience :: Developers',
          'Intended Audience :: System Administrators',
          'License :: OSI Approved :: GNU Lesser General Public License v2 (LGPLv2)'
          'Operating System :: POSIX',
          'Programming Language :: Python :: 2.7',
          'Topic :: Software Development :: Libraries :: Application Frameworks'
      ],
      packages=['alienvault-api-core',
                'alienvault-api-core.celerymethods',
                'alienvault-api-core.celerymethods.tasks',
                'alienvault-api-core.celerymethods.jobs',
                'alienvault-api-core.apimethods.sensor',
                'alienvault-api-core.apimethods.sensor.exceptions',
                'alienvault-api-core.apimethods',
                'alienvault-api-core.apimethods.system',
                'alienvault-api-core.apimethods.system.exceptions',
                'alienvault-api-core.apimethods.host',
                'alienvault-api-core.apimethods.otx',
                'alienvault-api-core.apimethods.auth',
                'alienvault-api-core.apimethods.server',
                'alienvault-api-core.apimethods.data',
                'alienvault-api-core.apimethods.apps.plugins',
                'alienvault-api-core.apimethods.apps',
                'alienvault-api-core.apimethods.plugin',
                'alienvault-api-core.apiexceptions',
                'alienvault-api-core.db.models',
                'alienvault-api-core.db',
                'alienvault-api-core.db.redis',
                'alienvault-api-core.db.methods',
                'alienvault-api-core.ansiblemethods.sensor',
                'alienvault-api-core.ansiblemethods.app',
                'alienvault-api-core.ansiblemethods',
                'alienvault-api-core.ansiblemethods.system',
                'alienvault-api-core.ansiblemethods.server',
                ],
      package_dir={'alienvault-api-core': 'src'},
      scripts=['scripts/about',
               'scripts/add_system',
               'scripts/external_dns',
               'scripts/get_backup',
               'scripts/get_network',
               'scripts/get_registered_systems',
               'scripts/get_connected_sensors',
               'scripts/internet_connectivity',
               'scripts/make_tunnel_with_vpn',
               'scripts/rawlogscleaner',
               'scripts/register_appliance',
               'scripts/restore_backup',
               'scripts/set_network_interface',
               'scripts/support_tool',
               'scripts/support_tunnel',
               'scripts/synchronise_sensor_detectors',
               'scripts/system_id',
               'scripts/systems']
      )
