function DFastICA_Unload
    global bFastICAReady;
    unloadlibrary('FastICA');
    bFastICAReady = false;
end