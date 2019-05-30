@rem  An OS/2 command file to add the SOMBank
@rem  server definitions to the implementation Repository.
@rem  If any of these servers will be located on another host,
@rem  please modify this file appropriately before it is
@rem  executed.
@rem
@rem  For example, if the ServerBroker was to be located
@rem  on a host named SvrBrkr, then the first line below should
@rem  be modified to look like this:
@rem
@rem       regimpl -A -i ServerBroker -h SvrBrkr
@rem
@rem
regimpl -A -i ServerBroker 
regimpl -A -i CentralDBServer
regimpl -A -i Central -v BankSOMDServer
regimpl -A -i B001DBServer
regimpl -A -i B001 -v BankSOMDServer
regimpl -A -i B002DBServer
regimpl -A -i B002 -v BankSOMDServer
regimpl -A -i B003DBServer
regimpl -A -i B003 -v BankSOMDServer
@rem
@rem  If additional branches are desired, duplicate the
@rem  last two lines above and alter the Bxxx strings to
@rem  specify the new Branch ID(s).
@rem
