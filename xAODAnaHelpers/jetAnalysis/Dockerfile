FROM lukasheinrich/atlas-analysisbase-2.3.38
RUN mkdir /analysis                                                                                          
WORKDIR /analysis                                                                                            
ADD xAODBTaggingEfficiency-00-00-26 xAODBTaggingEfficiency
ADD install.sh install.sh                                                                                    
RUN yum install -y wget
RUN ./install.sh                                                                                             
ADD run run
