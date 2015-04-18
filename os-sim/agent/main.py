#!/usr/bin/python

from Agent import Agent

if __name__ == '__main__':
    
    # Init agent and read config
    agent = Agent()
    agent.parseConfig()

    # connect to server
    agent.connect()

    # parse
    agent.parser()
    

