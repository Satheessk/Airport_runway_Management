import streamlit as st
import pandas as pd
import datetime
import time

# --- Configuration & Theme Setup ---
st.set_page_config(page_title="Airport Runway Manager", page_icon="✈️", layout="wide")

st.title("✈️ Airport Runway Management System")
st.caption("A Priority Queue Max-Heap Simulation tracking live plane landings.")

# --- Initialize Session States (Persistent Application Memory) ---
if "queue" not in st.session_state:
    st.session_state.queue = []  # List of dicts representing our planes
if "plane_id_counter" not in st.session_state:
    st.session_state.plane_id_counter = 1
if "arrival_time_counter" not in st.session_state:
    st.session_state.arrival_time_counter = 1

# --- Priority Queue Logic Functions ---
def get_priority_key(plane):
    """
    Simulates Max-Heap priority.
    Higher priority items come first:
    1. Emergency Status (1 beats 0)
    2. Lower Fuel Level (Lower values beat higher values)
    3. Arrival Time (Earlier arrivals beat later arrivals)
    """
    return (plane["emergency"], -plane["fuel_level"], -plane["arrival_time"])

def sort_queue():
    st.session_state.queue.sort(key=get_priority_key, reverse=True)

def append_to_file(log_text):
    with open("runway_report.txt", "a") as f:
        f.write(log_text + "\n")

# --- UI Sidebar: Input Panel ---
st.sidebar.header("🛫 Control Tower Entrances")

with st.sidebar.form(key="plane_form", clear_on_submit=True):
    fuel = st.number_input("Enter Fuel Level (1-100):", min_value=1, max_value=100, value=80)
    
    # Auto emergency check logic matching your C code
    auto_emergency = fuel < 10
    if auto_emergency:
        st.info("⚠️ Fuel < 10: Locked to Emergency Status Automatically.")
        emergency_input = 1
    else:
        emergency_choice = st.radio("Is it an emergency landing?", ("No", "Yes"))
        emergency_input = 1 if emergency_choice == "Yes" else 0
        
    submit_button = st.form_submit_button(label="Enqueue Flight")

if submit_button:
    new_plane = {
        "id": st.session_state.plane_id_counter,
        "fuel_level": fuel,
        "arrival_time": st.session_state.arrival_time_counter,
        "emergency": emergency_input,
        "canceled": 0
    }
    st.session_state.queue.append(new_plane)
    sort_queue()
    
    st.sidebar.success(f"Plane {st.session_state.plane_id_counter} safely added to stack!")
    st.session_state.plane_id_counter += 1
    st.session_state.arrival_time_counter += 1

# --- Main Layout: Columns for Actions and Tracking ---
col1, col2 = st.columns([2, 1])

with col2:
    st.subheader("⚙️ Air Control Operations")
    
    # Action 1: Dequeue/Land Plane
    if st.button("🛬 Dequeue Next Plane for Landing", use_container_width=True):
        landed = False
        while len(st.session_state.queue) > 0:
            next_flight = st.session_state.queue.pop(0) # Pull top priority item
            
            if next_flight["canceled"] == 1:
                st.warning(f"Plane {next_flight['id']} landing was canceled. Skipping track path...")
                continue
                
            st.balloons()
            st.success(f"🎉 Success! Plane {next_flight['id']} has touched down safely.")
            
            # Log landing to historic file
            now = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            log_str = f"[LANDED] Date/Time: {now} -> Plane ID: {next_flight['id']} | Fuel: {next_flight['fuel_level']} | Emergency: {'YES' if next_flight['emergency'] else 'NO'}"
            append_to_file(log_str)
            landed = True
            break
            
        if not landed:
            st.info("The runway is currently free. No planes waiting.")

    st.markdown("---")
    
    # Action 2: Cancel Flight Clearance
    st.markdown("#### ❌ Cancel Flight Runway Clearance")
    cancel_id = st.number_input("Enter Plane ID to cancel:", min_value=1, step=1)
    if st.button("Cancel Landing Clearance", type="primary", use_container_width=True):
        found = False
        for p in st.session_state.queue:
            if p["id"] == cancel_id:
                p["canceled"] = 1
                st.error(f"Plane {cancel_id} clearance canceled.")
                found = True
                break
        if not found:
            st.warning(f"Plane ID {cancel_id} not found in live queue.")

    st.markdown("---")
    
    # Action 3: Save Operational State
    if st.button("💾 Save Operational Layout & Logs", use_container_width=True):
        today = datetime.date.today().strftime("%d-%m-%m-%Y")
        header = f"\n=========================================================\n" \
                 f"AIRPORT RUNWAY DATA LOG - DATE: {today}\n" \
                 f"=========================================================\n" \
                 f"Planes Currently Waiting in Queue (Snapshot at Save):\n" \
                 f"Total in Queue: {len(st.session_state.queue)}\n" \
                 f"---------------------------------------------------------\n" \
                 f"ID\tFuel\tArrival\tEmergency\tCanceled\n" \
                 f"---------------------------------------------------------\n"
        
        body = ""
        if len(st.session_state.queue) == 0:
            body += "(No planes waiting in queue)\n"
        else:
            for p in st.session_state.queue:
                body += f"{p['id']}\t{p['fuel_level']}\t{p['arrival_time']}\t{p['emergency']}\t\t{p['canceled']}\n"
        body += "---------------------------------------------------------\n"
        
        append_to_file(header + body)
        st.toast("System logs appended to runway_report.txt successfully!", icon="💾")

with col1:
    st.subheader("📊 Live Runway Traffic Queue")
    
    # Filter active and show visual indicators
    if len(st.session_state.queue) == 0:
        st.info("No planes currently circling the airport airspace.")
    else:
        # Convert queue tracking matrix into a clean, pretty Pandas DataFrame
        df = pd.DataFrame(st.session_state.queue)
        
        # Format the visual data columns to look pristine for a user interface
        df["Emergency Status"] = df["emergency"].apply(lambda x: "🚨 CRITICAL" if x == 1 else "Normal")
        df["Flight Status"] = df["canceled"].apply(lambda x: "❌ CANCELED" if x == 1 else "⏳ Active Waiting")
        df = df.rename(columns={"id": "Plane ID", "fuel_level": "Fuel %", "arrival_time": "Arrival Sequence"})
        
        # Display the table, dropping technical sorting raw key columns from human view
        st.dataframe(
            df[["Plane ID", "Fuel %", "Arrival Sequence", "Emergency Status", "Flight Status"]],
            use_container_width=True,
            hide_index=True
        )
        
        # Live Peek logic window block directly underneath the chart
        active_peek = [p for p in st.session_state.queue if p["canceled"] == 0]
        if active_peek:
            next_up = active_peek[0]
            st.markdown(f"""
            <div style="background-color:#1E293B; padding:15px; border-radius:10px; border-left: 5px solid #F59E0B;">
                <h4 style="margin:0; color:#F59E0B;">🎯 Next Scheduled Landing Target</h4>
                <p style="margin:5px 0 0 0; font-size:16px;">
                    <b>Plane ID:</b> {next_up['id']} | <b>Fuel Level:</b> {next_up['fuel_level']}% | 
                    <b>Priority:</b> {'🚨 EMERGENCY STATUS' if next_up['emergency'] else 'Standard Tier'}
                </p>
            </div>
            """, unsafe_allow_html=True)