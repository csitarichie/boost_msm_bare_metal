#include "qp_port.h"
#include "dpp.h"
#include "bsp.h"

// back-end
#include "msm/back/state_machine.hpp"
#include "msm/back/metafunctions.hpp"
//front-end
#include "msm/front/state_machine_def.hpp"
// Quantum Leaps Spy librarry
#include "qp_port.h"


// Local-scope objects -------------------------------------------------------
static QEvent const *l_tableQueueSto[N_PHILO];
static QEvent const *l_philoQueueSto[N_PHILO][N_PHILO];
static QEvent const *l_lwIPMgrQueueSto[10];
static QSubscrList   l_subscrSto[MAX_PUB_SIG];

static union SmallEvents {
    void *min_size;
    TableEvt te;
    // other event types to go into this pool
} l_smlPoolSto[2*N_PHILO];                 // storage for the small event pool

static union MediumEvent {
    void *min_size;
    QEvent qe;
    TextEvt te;
    // other event types to go into this pool
} l_medPoolSto[4];                        // storage for the medium event pool

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace
{

    enum MsmQSRecordTypes {
    	 QS_MSM_STATE = QS_USER
    	,QS_MSM_STATE_ENTRY
    	,QS_MSM_STATE_EXIT
    	,QS_MSM_NO_TRANS
    	,QS_MSM_GUARD
    	,QS_MSM_ACTION
    };
    // events
    struct play {};
    struct end_pause {};
    struct stop {};
    struct pause {};
    struct open_close {};

    // A "complicated" event type that carries some data.
	enum DiskTypeEnum
    {
        DISK_CD=0,
        DISK_DVD=1
    };
    struct cd_detected
    {
        cd_detected(/* std::string name,*/ DiskTypeEnum diskType)
            : /* name(name), */
            disc_type(diskType)
        {}

        /* std::string name; */
        DiskTypeEnum disc_type;
    };

    // front-end: define the FSM structure 
    struct player_ : public msm::front::state_machine_def<player_>
    {
    	typedef int no_exception_thrown;
    	typedef int no_message_queue;
    	typedef ::boost::mpl::vector2< no_exception_thrown, no_message_queue >               configuration;
    	//typedef ::boost::mpl::vector1< no_exception_thrown >               configuration;
        // The list of FSM states
        struct Empty : public msm::front::state<> 
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {/* std::cout << "entering: Empty" << std::endl; */}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {/*std::cout << "leaving: Empty" << std::endl;*/}
        };
        struct Open : public msm::front::state<> 
        {	 
            template <class Event,class FSM>
            void on_entry(Event const& ,FSM&) {/* std::cout << "entering: Open" << std::endl; */}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {/* std::cout << "leaving: Open" << std::endl; */ }
        };

        // sm_ptr still supported but deprecated as functors are a much better way to do the same thing
        struct Stopped : public msm::front::state<msm::front::default_base_state,msm::front::sm_ptr> 
        {	 
            template <class Event,class FSM>
            void on_entry(Event const& ,FSM&) {/* std::cout << "entering: Stopped" << std::endl; */}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {/*std::cout << "leaving: Stopped" << std::endl;*/}
            void set_sm_ptr(player_* pl)
            {
                m_player=pl;
            }
            player_* m_player;
        };

        struct Playing : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {/* std::cout << "entering: Playing" << std::endl; */}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {/* std::cout << "leaving: Playing" << std::endl; */}
        };

        // state not defining any entry or exit
        struct Paused : public msm::front::state<>
        {
        };

        // the initial state of the player SM. Must be defined
        typedef Empty initial_state;

        // transition actions
        void start_playback(play const& event_)
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::start_playback));
        	QS_OBJ(&event_);
        	QS_END();
        }

        void open_drawer(open_close const& event_)
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::open_drawer));
        	QS_OBJ(&event_);
        	QS_END();
        }

        void close_drawer(open_close const& event_)
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::close_drawer));
        	QS_OBJ(&event_);
        	QS_END();
        }

        void store_cd_info(cd_detected const& event_)
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::store_cd_info));
        	QS_OBJ(&event_);
        	QS_END();
        }

        void stop_playback(stop const& event_)
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::stop_playback));
        	QS_OBJ(&event_);
        	QS_END();
        }

        void pause_playback(pause const& event_)
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::pause_playback));
        	QS_OBJ(&event_);
        	QS_END();
        }

        void resume_playback(end_pause const& event_)
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::resume_playback));
        	QS_OBJ(&event_);
        	QS_END();
        }

        void stop_and_open(open_close const& event_)
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::stop_and_open));
        	QS_OBJ(&event_);
        	QS_END();
        }

        void stopped_again(stop const& event_ )
        {
        	QS_BEGIN(QS_MSM_ACTION, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::stopped_again));
        	QS_OBJ(&event_);
        	QS_END();
        }
        // guard conditions
        bool good_disk_format(cd_detected const& evt)
        {
        	QS_BEGIN(QS_MSM_GUARD, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::good_disk_format));
        	QS_OBJ(&evt);
        	QS_END();
            // to test a guard condition, let's say we understand only CDs, not DVD
            if (evt.disc_type != DISK_CD)
            {
                /* std::cout << "wrong disk, sorry" << std::endl; */
                return false;
            }
            return true;
        }
        // used to show a transition conflict. This guard will simply deactivate one transition and thus
        // solve the conflict
        bool auto_start(cd_detected const& event_)
        {
        	QS_BEGIN(QS_MSM_GUARD, this);
        	QS_OBJ(this);
        	QS_FUN(reinterpret_cast<void*>(&::player_::auto_start));
        	QS_OBJ(&event_);
        	QS_END();
            return false;
        }

        typedef player_ p; // makes transition table cleaner

        // Transition table for player
        struct transition_table : mpl::vector<
            //    Start     Event         Next      Action				 Guard
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Stopped , play        , Playing , &p::start_playback                         >,
          a_row < Stopped , open_close  , Open    , &p::open_drawer                            >,
           _row < Stopped , stop        , Stopped                                              >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Open    , open_close  , Empty   , &p::close_drawer                           >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Empty   , open_close  , Open    , &p::open_drawer                            >,
            row < Empty   , cd_detected , Stopped , &p::store_cd_info   ,&p::good_disk_format  >,
            row < Empty   , cd_detected , Playing , &p::store_cd_info   ,&p::auto_start        >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Playing , stop        , Stopped , &p::stop_playback                          >,
          a_row < Playing , pause       , Paused  , &p::pause_playback                         >,
          a_row < Playing , open_close  , Open    , &p::stop_and_open                          >,
            //  +---------+-------------+---------+---------------------+----------------------+
          a_row < Paused  , end_pause   , Playing , &p::resume_playback                        >,
          a_row < Paused  , stop        , Stopped , &p::stop_playback                          >,
          a_row < Paused  , open_close  , Open    , &p::stop_and_open                          >
            //  +---------+-------------+---------+---------------------+----------------------+
        > {};
        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
//            std::cout << "no transition from state " << state
//                << " on event " << typeid(e).name() << std::endl;
        }
    };
    // Pick a back-end
    //typedef msm::back::state_machine<player_> player;

    struct player : msm::back::state_machine<player_> {
    	player() : msm::back::state_machine<player_>()
    	{
    	   player_::Empty* sEmpty(get_state<player_::Empty*>());
    	   player_::Open* sOpen(get_state<player_::Open*>());
    	   player_::Stopped* sStopped(get_state<player_::Stopped*>());
    	   player_::Paused* sPaused(get_state<player_::Paused*>());
    	   player_::Playing* sPlaying(get_state<player_::Playing*>());

    	   QS_OBJ_DICTIONARY(sPaused);
    	   QS_OBJ_DICTIONARY(sStopped);
    	   QS_OBJ_DICTIONARY(sOpen);
    	   QS_OBJ_DICTIONARY(sEmpty);
    	   QS_OBJ_DICTIONARY(sPlaying);

    	   // void (::player::* aStart_playback)(play const& event_);

    	   //int tmp(&::player::start_playback);

    	   void* aStart_playback   (reinterpret_cast<void*>(&::player::start_playback));
    	   void* aOpen_drawer      (reinterpret_cast<void*>(&::player::open_drawer));
    	   void* aClose_drawer     (reinterpret_cast<void*>(&::player::close_drawer));
    	   void* aStore_cd_info    (reinterpret_cast<void*>(&::player::store_cd_info));
    	   void* aStop_playback    (reinterpret_cast<void*>(&::player::stop_playback));
    	   void* aPause_playback   (reinterpret_cast<void*>(&::player::pause_playback));
    	   void* aStop_and_open    (reinterpret_cast<void*>(&::player::stop_and_open));
    	   void* aStopped_again    (reinterpret_cast<void*>(&::player::stopped_again));
    	   void* aGood_disk_format (reinterpret_cast<void*>(&::player::good_disk_format));
    	   void* aAuto_start       (reinterpret_cast<void*>(&::player::auto_start));
    	   void* aResume_playback  (reinterpret_cast<void*>(&::player::resume_playback));


    	   //QS_FUN_DICTIONARY(&no_transition);
    	   QS_FUN_DICTIONARY(aStart_playback);
    	   QS_FUN_DICTIONARY(aOpen_drawer);
    	   QS_FUN_DICTIONARY(aClose_drawer);
    	   QS_FUN_DICTIONARY(aStore_cd_info);
    	   QS_FUN_DICTIONARY(aStop_playback);
    	   QS_FUN_DICTIONARY(aPause_playback);
    	   QS_FUN_DICTIONARY(aStop_and_open);
    	   QS_FUN_DICTIONARY(aStopped_again);
    	   QS_FUN_DICTIONARY(aGood_disk_format);
    	   QS_FUN_DICTIONARY(aAuto_start);
    	   QS_FUN_DICTIONARY(aResume_playback);
    	}

    };

    //
    // Testing utilities.
    //
    // static char const* const state_names[] = { "Stopped", "Open", "Empty", "Playing", "Paused" };

    void pstate(player const& p)
    {
    	QS_BEGIN( QS_MSM_STATE, &p);
    	QS_OBJ(&p);
    	QS_OBJ(p.get_state_by_id((p.current_state()[0])));
    	QS_END();
        //std::cout << " -> " << state_names[p.current_state()[0]] << std::endl;
    }

    void test()
    {        
		player msmPlayer;
		open_close  eOpenClose;
		cd_detected eCdDetectedDiskDvd(DISK_DVD);
		cd_detected eCdDetectedDiskCD(DISK_CD);
		play        ePlay;
		pause       ePause;
		end_pause   eEndPause;
		stop        eStop;

		QS_OBJ_DICTIONARY(&msmPlayer);
		QS_OBJ_DICTIONARY(&eOpenClose);
		QS_OBJ_DICTIONARY(&eCdDetectedDiskDvd);
		QS_OBJ_DICTIONARY(&eCdDetectedDiskCD);
		QS_OBJ_DICTIONARY(&ePlay);
		QS_OBJ_DICTIONARY(&ePause);
		QS_OBJ_DICTIONARY(&eEndPause);
		QS_OBJ_DICTIONARY(&eStop);

        // needed to start the highest-level SM. This will call on_entry and mark the start of the SM
        msmPlayer.start();
        pstate(msmPlayer);

        // go to Open, call on_exit on Empty, then action, then on_entry on Open
        msmPlayer.process_event(eOpenClose);
        pstate(msmPlayer);

        msmPlayer.process_event(eOpenClose);
        pstate(msmPlayer);
        // will be rejected, wrong disk type
        msmPlayer.process_event(eCdDetectedDiskDvd);
        pstate(msmPlayer);

        msmPlayer.process_event(eCdDetectedDiskCD);
        pstate(msmPlayer);

		msmPlayer.process_event(ePlay);
		pstate(msmPlayer);
        // at this point, Play is active      
        msmPlayer.process_event(ePause);
        pstate(msmPlayer);

        // go back to Playing
        msmPlayer.process_event(eEndPause);
        pstate(msmPlayer);

        msmPlayer.process_event(eEndPause);
        pstate(msmPlayer);

        msmPlayer.process_event(eStop);
        pstate(msmPlayer);
        // event leading to the same state
        // no action method called as it is not present in the transition table
        msmPlayer.process_event(eStop);
        pstate(msmPlayer);
    }
}


//............................................................................
int main(void) {

    BSP_init();                                          // initialize the BSP

    QF::init();       // initialize the framework and the underlying RT kernel

                                                     // object dictionaries...
    QS_OBJ_DICTIONARY(l_smlPoolSto);
    QS_OBJ_DICTIONARY(l_medPoolSto);
    QS_OBJ_DICTIONARY(l_lwIPMgrQueueSto);
    QS_OBJ_DICTIONARY(l_philoQueueSto[0]);
    QS_OBJ_DICTIONARY(l_philoQueueSto[1]);
    QS_OBJ_DICTIONARY(l_philoQueueSto[2]);
    QS_OBJ_DICTIONARY(l_philoQueueSto[3]);
    QS_OBJ_DICTIONARY(l_philoQueueSto[4]);
    QS_OBJ_DICTIONARY(l_tableQueueSto);

    QF::psInit(l_subscrSto, Q_DIM(l_subscrSto));     // init publish-subscribe

                                                  // initialize event pools...
    QF::poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
    QF::poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));

                                                // start the active objects...
    /* AO_LwIPMgr->start((uint8_t)1,
                    l_lwIPMgrQueueSto, Q_DIM(l_lwIPMgrQueueSto),
                    (void *)0, 0, (QEvent *)0);
    uint8_t n;
    for (n = 0; n < N_PHILO; ++n) {
        AO_Philo[n]->start((uint8_t)(n + 2),
                           l_philoQueueSto[n], Q_DIM(l_philoQueueSto[n]),
                           (void *)0, 0, (QEvent *)0);
    }
    AO_Table->start((uint8_t)(N_PHILO + 2),
                    l_tableQueueSto, Q_DIM(l_tableQueueSto),
                    (void *)0, 0, (QEvent *)0); */

    test();

    QF::run();                                       // run the QF application

    return 0;
}

