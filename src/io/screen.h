#pragma once

void initScreen();

void updateVolumeUI(int volume);
void updateMutedUI(bool muted);
void updateMicUI(int micLoudness);
void updateWaveformUI(int micLoudness);

void updatePlayPauseButtonUI(bool pressed);
void updateSkipBackButtonUI(bool pressed);
void updateSkipNextButtonUI(bool pressed);
void updateMicMuteButtonUI(bool micMuted);
void updateMicGainUI(int micSensitivity);
void updateModeUI(int mode);