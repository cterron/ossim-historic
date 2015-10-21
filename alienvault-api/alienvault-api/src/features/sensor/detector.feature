Feature: Sensor detector operations

  Scenario: Get the plugin list within the config.yml
    Given I set username and password to ghost administrator
    And I log into the ossim API using "https://127.0.0.1:40011/av/api/1.0/auth/login"
    When I send a GET request to url "https://127.0.0.1:40011/av/api/1.0/sensor/local/plugins"
    Then The http status code must be "200"
    And JSON response has key "status" and value equals to string "success"
    And JSON response has key "data.plugins"
	And JSON response has key "data.plugins.AE298B1A-AF3F-11E3-9452-C242E4CCA549"
    #And JSON key "data.plugins" has the value "{"AE298B1A-AF3F-11E3-9452-C242E4CCA548":{ "cisco-asa": "cpe:/a:cpe_data", "pam_unix": "cpe:/a:cpe_data", "ssh": "cpe:/a:cpe_data" }, "AE298B1A-AF3F-11E3-9452-C242E4CCA549": { "apache": "cpe:/a:cpe_data", "cisco-pix": "cpe:/a:cpe_data"}}" 

  @wip
  Scenario: Set a new plugin list - with fake plugins parameter
    Given I set username and password to ghost administrator
    And I log into the ossim API using "https://127.0.0.1:40011/av/api/1.0/auth/login"
    And I add POST data key "plugins","ll"
    When I send a POST request to url "https://127.0.0.1:40011/av/api/1.0/sensor/local/plugins"
    Then The http status code must be "400"
    And JSON response has key "status" and value equals to string "error"
  @wip
  Scenario: Set a new plugin list - with fake plugin
    Given I set username and password to ghost administrator
    And I log into the ossim API using "https://127.0.0.1:40011/av/api/1.0/auth/login"
    And I add POST data key "plugins","{"AE298B1A-AF3F-11E3-9452-C242B0EB0D28":{},"AE298B1A-AF3F-11E3-9452-C242E4CCA548":{"pam_unix-fake":"cpe:/a:cpe_data","ssh":"cpe:/a:cpe_data"}}"
    When I send a POST request to url "https://127.0.0.1:40011/av/api/1.0/sensor/local/plugins"
    Then The http status code must be "500"
    And JSON response has key "status" and value equals to string "error"
    And JSON response has key "message" and value equals to string "Error setting sensor detector plugins: Plugin path /etc/ossim/agent/plugins/pam_unix-fake.cfg doesn't exist!"

  Scenario: Set a new plugin list
    Given I set username and password to ghost administrator
    And I log into the ossim API using "https://127.0.0.1:40011/av/api/1.0/auth/login"
    And I add POST data key "plugins","{"00000000-0000-0000-0000-00000000AAAA":{"apache":"cpe:/a:cpe_data","cisco-asa":"cpe:/a:cpe_data"},"00000000-0000-0000-0000-00000000BBBB":{"pam_unix":"cpe:/a:cpe_data","ssh":"cpe:/a:cpe_data"}}"
    When I send a POST request to url "https://127.0.0.1:40011/av/api/1.0/sensor/local/plugins"
    Then The http status code must be "200"
    And JSON response has key "status" and value equals to string "success"
