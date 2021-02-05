/* See LICENSE file for copyright and license details. */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "../util.h"

#if defined(__OpenBSD__)
	#include <sys/audioio.h>

	

	const char *
	vol_perc(const char *card)
	{
		static int cls = -1;
		mixer_devinfo_t mdi;
		mixer_ctrl_t mc;
		int afd = -1, m = -1, v = -1;

		if ((afd = open(card, O_RDONLY)) < 0) {
			warn("open '%s':", card);
			return NULL;
		}

		for (mdi.index = 0; cls == -1; mdi.index++) {
			if (ioctl(afd, AUDIO_MIXER_DEVINFO, &mdi) < 0) {
				warn("ioctl 'AUDIO_MIXER_DEVINFO':");
				close(afd);
				return NULL;
			}
			if (mdi.type == AUDIO_MIXER_CLASS &&
			    !strncmp(mdi.label.name,
				     AudioCoutputs,
				     MAX_AUDIO_DEV_LEN))
				cls = mdi.index;
			}
		for (mdi.index = 0; v == -1 || m == -1; mdi.index++) {
			if (ioctl(afd, AUDIO_MIXER_DEVINFO, &mdi) < 0) {
				warn("ioctl 'AUDIO_MIXER_DEVINFO':");
				close(afd);
				return NULL;
			}
			if (mdi.mixer_class == cls &&
			    ((mdi.type == AUDIO_MIXER_VALUE &&
			      !strncmp(mdi.label.name,
				       AudioNmaster,
				       MAX_AUDIO_DEV_LEN)) ||
			     (mdi.type == AUDIO_MIXER_ENUM &&
			      !strncmp(mdi.label.name,
				      AudioNmute,
				      MAX_AUDIO_DEV_LEN)))) {
				mc.dev = mdi.index, mc.type = mdi.type;
				if (ioctl(afd, AUDIO_MIXER_READ, &mc) < 0) {
					warn("ioctl 'AUDIO_MIXER_READ':");
					close(afd);
					return NULL;
				}
				if (mc.type == AUDIO_MIXER_VALUE)
					v = mc.un.value.num_channels == 1 ?
					    mc.un.value.level[AUDIO_MIXER_LEVEL_MONO] :
					    (mc.un.value.level[AUDIO_MIXER_LEVEL_LEFT] >
					     mc.un.value.level[AUDIO_MIXER_LEVEL_RIGHT] ?
					     mc.un.value.level[AUDIO_MIXER_LEVEL_LEFT] :
					     mc.un.value.level[AUDIO_MIXER_LEVEL_RIGHT]);
				else if (mc.type == AUDIO_MIXER_ENUM)
					m = mc.un.ord;
			}
		}

		close(afd);

		return bprintf("%d", m ? 0 : v * 100 / 255);
	}
#else
	#include <sys/soundcard.h>
	#include <alsa/asoundlib.h>
	#include <alsa/mixer.h>
	
	const char*	
	vol_perc()
	{
	  const char* MIXER = "Master";

	  float vol = 0;
	  long pmin, pmax, pvol;
	  int isnotmuted = 0;
	
	  snd_mixer_t *handle;
	  snd_mixer_selem_id_t *sid;
	  snd_mixer_elem_t *elem;
	  snd_mixer_selem_id_alloca(&sid);

	  if(snd_mixer_open(&handle, 0) < 0)
	    return 0;

	  if(snd_mixer_attach(handle, "default") < 0
	     || snd_mixer_selem_register(handle, NULL, NULL) < 0
	     || snd_mixer_load(handle) > 0)
	    {
	      snd_mixer_close(handle);
	      return 0;
	    }

	  for(elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next(elem))
	    {
	      snd_mixer_selem_get_id(elem, sid); 
	      if(!strcmp(snd_mixer_selem_id_get_name(sid), MIXER))
		{
		  
		  snd_mixer_selem_get_playback_switch(elem, (snd_mixer_selem_channel_id_t) 0,&isnotmuted);	
		  snd_mixer_selem_get_playback_volume_range(elem, &pmin, &pmax);
		  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &pvol);
		  vol = ((float)pvol / (float)(pmax - pmin)) * 100;
		  break;
		}
	    }

	  snd_mixer_close(handle);
	  int tmp = (int)vol;
	  if(tmp%10 == 9)
		  tmp+=1;
	  if(isnotmuted){
		if (tmp <= 25)
			return bprintf("󰕿 %d%%", tmp);
	  	else if (tmp <= 75)
			return bprintf("󰖀 %d%%", tmp);
	  	else if (tmp <= 100)  
			return bprintf("󰕾 %d%%", tmp);
		else return bprintf("^c#7f0d07^󰕾 %d%%", tmp);
	  } else
	  	return bprintf("󰖁 %d%%", tmp);
	}
	
#endif
