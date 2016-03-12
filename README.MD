# 7DRL Violation: Escape

My entry for [7DRL 2016 competition](http://7drl.org).

Coffee-break almost-zero-combat roguelike about an android maid accused in murder of her master. Get a fake ID, escape the city on a train, don't get caught.

Had to dust off my C++ skills (sorry, no makefile), picked `ncurses` (OS X) and `pdcurses` (Windows) for drawing. I used VS 2015 (free version) and XCode to build this, didn't include project files, but it should be pretty straightforward.

For the sake of history this repo should have tag `7drl` for the last commit I managed to push before the deadline.

Due to a lack of time game turned out to be like ®Watchdogs™ cause you use your phone for everything except stunning police officers.

Here's unedited pitch document I wrote while preparing to the competition (with bad grammar etc.):

---

Violation: Escape
===

You're an android-maid on the run framed for a murder.

You need to escape the city as soon as possible.

Change your outfit
---

- Sort of tutorial

First of all you need to change your clothes, cause you look suspicios.

Get money
---

- Stealing physically
- Stealing digitally (hacking)
- Get credit on your fake ID

To use your phone you must be close to a private satelite if you want to stay unnoticed. The more you use public network the quicker they'll find out your exact location.

You can throw away your phone so police tracks only its location.

Acquire ID
---

- Buy from smugglers (takes around 30 hours)
- Steal one (requires an empty phone-carrier)

Your id is definitely useless to buy tickets out of here. You need to get new one and there's no legal way to do that. Enter the shadow part of the city. Visit a bar to contact mafia.

Buy tickets
---

- Instant buy
- Takes 48 hours to confirm your ID

Due to increased amount of android rebelion the border is well-protected and you need 2 days to confirm your identity after you buy tickets.

Attend train
---

Just be present at the station on time.

---

Gameplay mechanics
---

#### Paying for stuff

You pay with your ID, whenever you change your ID you lose all money attached to that.

In public (legal) places there's a lag between selling you something and actually checking if you have money. Paying for stuff with non-existing money for any of your fake IDs is a medium law violation, but it actually makes you lose the game if you get caught (and fail to run away instantly).

With money check comes information on your location when you were paying. So if you have any law violation on your id, police gonna inspect that place and try to find you.

#### Combat

The whole game is almost zero-combat except moments where you have to avoid being captured or a fight. When you have to protect your life you got a choice to either use an ordinary human power (to not cause any suspicion in case your ID is human or violation minor) or android extra-force and a choice between running away or knocking your opponent out.

- Knock out
- Knock out (extra-force) [not available if there is any witnesses, cause it doesn't matter what to chose]
- Run away
- Run away (extra-speed)

So when a PO approaches you, you'd better move away from other people around so you have no witnesses.

####News

You can track police progress on the murder investigation in the news.

####Suspicion

You'd better look like a human being, cause it's one step farer than droid for you to be treated like a possible criminal.

####In-person vs remote

Most of the action you can perform through the network except:

- Get a carrier-phone with fake ID from smugglers
- Prolong your ID confirmation process in case of minor violation
- Steal an ID or money

####ID Confirmation

- If any minor law violation will be recorded while your ID is in process of confirmation it will be postponed until your personal visit

####Law violation

Whenever you do something wrong you have to give your ID to police officer so he records your crime.

Minor law violations just make it harder to execute certain actions (like traveling etc.) and don't result in locking an individual away.

- Insulting someone or breaking a fight

Medium law violations result in a short lock up with crime history checking.

- Opposing PO will without physical contact
- Violating curfew

Major law violations result in full ID check which in case of our hero result in investigation, complete loss of that ID and require you to run away from police.

- Physical opposition of PO

####Humans vs androids

Androids are much stronger and capable of comlicated computations. That means whenever an idividual is showing an exessive muscular force it means he's definitely an android.

Ex: If a police officer is checking your ID and he sees you're registered as a human you'd better not use any "superpowers" in case you need to run away cause that's gonna be a serious law violation.

####How ID works

You're connected to the network, unless you chose to disconnect. Everyone has its id programmed into a implanted chip. Everyone who's faking it is using a separate device with physical control (called "phone-carrier" in this game).

You can be registered as a human or an android. If you identity mismatch that's a serious violation of law.

####Charging

Your batteries constantly recharge. You got to use illegal repair stations (or legal if you have android ID card and only for charges, otherwise personnel might call a police).

####Fights with police

When you first disobey PO order he will start to approach you, if you increase the distance, PO will order you to stay idle, if you disobey PO will shoot you in the leg. If you surrender after that (wait for him to come over), he'll stop shooting. If you keep trying to escape, PO will warn you once more and then will try to finish you.

If you got a major law violation PO will shoot you at the first attempt.

PO weapon is ID-bound, you can't use it (haha), *I don't know where crazy androids take their weapons, maybe they're cops*

####Rogue androids

Sometimes androids can turn crazy and try to shoot people or take hostages. That's dangerous for you in two ways:

- You can get shot
- Police will check people IDs at the crime scene, if you have an android ID they'll take you for investigation for minor law violation (if you don't have any, otherwise it's gonna be medium)

####Injuries

When people see you injured they got a chance to spot that you have no blood. In that case a pack of human blood might be useful.

####Network usage

Everyone is connected to the network through her ID

#####Personal connection
Insecure and public, last resort, you can't throw away your body

#####Phone
Cause you're physically touching your phone there's no way to identify your original unique id.

Can throw away without turning off, so police tracks only its location


---

Usable items
---

- Bloodpack
- Phone