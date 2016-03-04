FROM lukasheinrich/atlas-analysisbase-2.3.38
RUN mkdir /analysis                                                                                          
WORKDIR /analysis                                                                                            
ADD install.sh install.sh                                                                                    
RUN ./install.sh                                                                                             
ADD simpleconfig.py simpleconfig.py                                                                          
ADD tests tests        
